require('dotenv').config()

var fs = require('fs');

const { Api, JsonRpc, RpcError, Serialize } = require('eosjs');
const { JsSignatureProvider } = require('eosjs/dist/eosjs-jssig');      // development only
//const AbiProvider = require('eosjs/src/abi.abi.json');                // development only

const fetch = require('node-fetch');                                    // node only; not needed in browsers
const { TextEncoder, TextDecoder } = require('util');                   // node only; native TextEncoder/Decoder

const signatureProvider = new JsSignatureProvider([process.env.MAIN_PRIV_KEY,process.env.BACKUP_PRIV_KEY]);

var api_endpoint = `${ process.env.EOSIO_PROTOCOL||"http"}://${ process.env.EOSIO_HOST||"127.0.0.1"}:${process.env.EOSIO_PORT||"8888"}`;

const rpc = new JsonRpc(api_endpoint, { fetch });

const api = new Api({ rpc, signatureProvider, textDecoder: new TextDecoder(), textEncoder: new TextEncoder() });

const MAIN_TRX_AUTH_TEMPLATE = {
    actor: `${process.env.MAIN_CONTRACT}`,
    permission: `${process.env.MAIN_PERMISSION||"active"}`,
}

const BACKUP_TRX_AUTH_TEMPLATE = {
    actor: `${process.env.BACKUP_CONTRACT}`,
    permission: `${process.env.BACKUP_PERMISSION||"active"}`,
}

async function executeUpgrade(){

    const wasm = fs.readFileSync(`${__dirname}/contract/delphioracle.wasm`).toString(`hex`)

    const buffer = new Serialize.SerialBuffer({
        textEncoder: api.textEncoder,
        textDecoder: api.textDecoder,
    })

    let abi = JSON.parse(fs.readFileSync(`${__dirname}/contract/delphioracle.abi`, `utf8`))
    const abiDefinition = api.abiTypes.get(`abi_def`)
    // need to make sure abi has every field in abiDefinition.fields
    // otherwise serialize throws
    abi = abiDefinition.fields.reduce(
        (acc, { name: fieldName }) =>
            Object.assign(acc, { [fieldName]: acc[fieldName] || [] }),
        abi)
    abiDefinition.serialize(buffer, abi)

    var obj = {
        actions: [
            { // copy data to backup contract (delphibackup - copydata)
                account: process.env.BACKUP_CONTRACT,
                name: 'copydata',
                authorization: [BACKUP_TRX_AUTH_TEMPLATE],
                data: {},
            },
            { // clear data from main contract (delphioracle - clear)
                account: process.env.MAIN_CONTRACT,
                name: 'clear',
                authorization: [MAIN_TRX_AUTH_TEMPLATE],
                data: {
                    pair: "eosusd"
                },
            },
            { // clear data from main contract (delphioracle - clear)
                account: process.env.MAIN_CONTRACT,
                name: 'clear',
                authorization: [MAIN_TRX_AUTH_TEMPLATE],
                data: {
                    pair: "eosbtc"
                },
            },
            { // clear data from main contract (delphioracle - clear)
                account: process.env.MAIN_CONTRACT,
                name: 'clear',
                authorization: [MAIN_TRX_AUTH_TEMPLATE],
                data: {
                    pair: "btccny"
                },
            },
            { // clear data from main contract (delphioracle - clear)
                account: process.env.MAIN_CONTRACT,
                name: 'clear',
                authorization: [MAIN_TRX_AUTH_TEMPLATE],
                data: {
                    pair: "btcusd"
                },
            },
            { // set code for main contract (eosio - setcode)
                account: 'eosio',
                name: 'setcode',
                authorization: [MAIN_TRX_AUTH_TEMPLATE],
                data: {
                    account: process.env.MAIN_CONTRACT,
                    vmtype: 0,
                    vmversion: 0,
                    code: wasm,
                },
            },
            { // set abi for main contract (eosio - setabi)
                account: 'eosio',
                name: 'setabi',
                authorization: [MAIN_TRX_AUTH_TEMPLATE],
                data: {
                    account: process.env.MAIN_CONTRACT,
                    abi: Buffer.from(buffer.asUint8Array()).toString(`hex`),
                },
            },
            { // configure main contract (delphioracle - configure)
                account: process.env.MAIN_CONTRACT,
                name: 'configure',
                authorization: [MAIN_TRX_AUTH_TEMPLATE],
                data: {
                    g: {
                        datapoints_per_instrument: 21,
                        bars_per_instrument: 30,
                        vote_interval: 10000,
                        write_cooldown: 55000000,
                        approver_threshold: 1,
                        approving_oracles_threshold:2,
                        approving_custodians_threshold:1,
                        minimum_rank: 105,
                        paid: 21,
                        min_bounty_delay: 604800,
                        new_bounty_delay: 259200
                    }
                },
            },
            { // migrate data to main contract (delphioracle - migratedata)
                account: process.env.MAIN_CONTRACT,
                name: 'migratedata',
                authorization: [MAIN_TRX_AUTH_TEMPLATE],
                data: {},
            },
        ]
    }

    console.log("executeUpgrade", JSON.stringify(obj, null, 2));

    const result = await api.transact(obj, {
        blocksBehind: 3,
        expireSeconds: 30,
    }).catch((err) => {
        if (err.json) console.error(`Error [upgrade]: ${err.json.error.what}`, JSON.stringify(err, null, 2));
        else console.error("Error [upgrade]:", JSON.stringify(err, null, 2));
        return {error: err};
    });

    return result;

}


async function run(){

    res = await executeUpgrade()

    if (!res.error) {
        console.log(JSON.stringify(res, null, 2));
    }

}

run();
