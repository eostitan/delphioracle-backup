#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>

using namespace eosio;

CONTRACT delphibackup : public contract {
   public:
      using contract::contract;

    //Types

    enum e_asset_type: uint16_t {
        fiat=1,
        cryptocurrency=2,
        erc20_token=3,
        eosio_token=4,
        equity=5,
        derivative=6,
        other=7
    };

    typedef uint16_t asset_type;

      // TABLES

      TABLE snapshot {
        uint64_t id;
        time_point timestamp;

        uint64_t primary_key() const {return id;}
      };

      TABLE stats {
        name owner; 
        time_point timestamp;
        uint64_t count;
        time_point last_claim;
        asset balance;

        uint64_t primary_key() const {return owner.value;}
        uint64_t by_count() const {return -count;}
      };

      TABLE opairs {
        uint64_t id;
        name name;

        uint64_t primary_key() const {return id;}
        uint64_t by_name() const {return name.value;}
      };

      TABLE npairs {
        bool active = false;
        bool bounty_awarded = false;
        bool bounty_edited_by_custodians = false;

        name proposer;
        name name;

        asset bounty_amount = asset(0, symbol("EOS", 4));

        std::vector<eosio::name> approving_custodians;
        std::vector<eosio::name> approving_oracles;

        symbol base_symbol;
        asset_type base_type;
        eosio::name base_contract;

        symbol quote_symbol;
        asset_type quote_type;
        eosio::name quote_contract;
        
        uint64_t quoted_precision;

        uint64_t primary_key() const {return name.value;}
      };

      TABLE datapoints {
        uint64_t id;
        name owner;
        uint64_t value;
        uint64_t median;
        time_point timestamp;

        uint64_t primary_key() const {return id;}
        uint64_t by_timestamp() const {return timestamp.elapsed.to_seconds();}
        uint64_t by_value() const {return value;}
      };

      // INDEXES

      typedef eosio::multi_index<"stats"_n, stats,
          indexed_by<"count"_n, const_mem_fun<stats, uint64_t, &stats::by_count>>> statstable;

      typedef eosio::multi_index<"pairs"_n, opairs> opairstable;
      typedef eosio::multi_index<"npairs"_n, npairs> npairstable;
      typedef eosio::multi_index<"snapshot"_n, snapshot> snapshottable;

      typedef eosio::multi_index<"datapoints"_n, datapoints,
        indexed_by<"value"_n, const_mem_fun<datapoints, uint64_t, &datapoints::by_value>>, 
        indexed_by<"timestamp"_n, const_mem_fun<datapoints, uint64_t, &datapoints::by_timestamp>>> datapointstable;

      // ACTIONS

      ACTION copydata();
      ACTION erasedata();

      using copydata_action = action_wrapper<"copydata"_n, &delphibackup::copydata>;
      using erasedata_action = action_wrapper<"erasedata"_n, &delphibackup::erasedata>;
};