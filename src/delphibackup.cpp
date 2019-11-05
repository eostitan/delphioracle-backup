#include <delphibackup.hpp>

ACTION delphibackup::copydata() {

  require_auth(_self);

  snapshottable snapshot(_self, _self.value);
  if (snapshot.begin() != snapshot.end()) {
    return;
  }

  statstable stats(name("delphioracle"), name("delphioracle").value);
  statstable _stats(_self, _self.value);

  auto gitr = stats.begin();
  while (gitr != stats.end()) {
    _stats.emplace(_self, [&](auto& s){
      s.owner = gitr->owner;
      s.timestamp = gitr->timestamp;
      s.count = gitr->count;
      s.last_claim = gitr->last_claim;
      s.balance = gitr->balance;
    });
    gitr++;
  }

  opairstable pairs(name("delphioracle"), name("delphioracle").value);
  npairstable _pairs(_self, _self.value);
  auto pitr = pairs.begin();
  while (pitr != pairs.end()) {
    print(pitr->name);

    _pairs.emplace(_self, [&](auto& p) {
      p.active = true;
      p.bounty_awarded = true;
      p.bounty_edited_by_custodians = true;
      p.proposer = name("delphioracle");
      p.name = pitr->name;
      p.bounty_amount = asset(0, symbol("EOS", 4));

      if( pitr->name == "eosusd"_n ) {
        p.base_symbol =  symbol("EOS", 4);
        p.base_type = e_asset_type::eosio_token;
        p.base_contract = "eosio.token"_n;
        p.quote_symbol = symbol("USD", 2);
        p.quote_type = e_asset_type::fiat;
        p.quote_contract = ""_n;
        p.quoted_precision = 2;
      } else if( pitr->name == "eosbtc"_n ) {
        p.base_symbol =  symbol("EOS", 4);
        p.base_type = e_asset_type::eosio_token;
        p.base_contract = "eosio.token"_n;
        p.quote_symbol = symbol("BTC", 8);
        p.quote_type = e_asset_type::cryptocurrency;
        p.quote_contract = ""_n;
        p.quoted_precision = 8;
      } else if( pitr->name == "btccny"_n ) {
        p.base_symbol =  symbol("BTC", 8);
        p.base_type = e_asset_type::cryptocurrency;
        p.base_contract = ""_n;
        p.quote_symbol = symbol("CNY", 2);
        p.quote_type = e_asset_type::fiat;
        p.quote_contract = ""_n;
        p.quoted_precision = 2;
      } else if( pitr->name == "btcusd"_n ) {
        p.base_symbol =  symbol("BTC", 8);
        p.base_type = e_asset_type::cryptocurrency;
        p.base_contract = ""_n;
        p.quote_symbol = symbol("USD", 2);
        p.quote_type = e_asset_type::fiat;
        p.quote_contract = ""_n;
        p.quoted_precision = 2;
      }
    });

    statstable pstats(name("delphioracle"), pitr->name.value);
    statstable _pstats(_self, pitr->name.value);
    auto sitr = pstats.begin();
    while (sitr != pstats.end()) {
      _pstats.emplace(_self, [&](auto& s){
        s.owner = sitr->owner;
        s.timestamp = sitr->timestamp;
        s.count = sitr->count;
        s.last_claim = sitr->last_claim;
        s.balance = sitr->balance;
      });
      sitr++;
    }

    datapointstable datapoints(name("delphioracle"), pitr->name.value);
    datapointstable _datapoints(_self, pitr->name.value);
    auto ditr = datapoints.begin();
    while (ditr != datapoints.end()) {
      _datapoints.emplace(_self, [&](auto& s){
        s.id = ditr->id;
        s.owner = ditr->owner;
        s.value = ditr->value;
        s.median = ditr->median;
        s.timestamp = ditr->timestamp;
      });
      ditr++;
    }

    pitr++;
  }

  snapshot.emplace(_self, [&](auto& s) {
    s.id = 0;
    s.timestamp = current_time_point();
  });

}

ACTION delphibackup::erasedata() {

  require_auth(_self);

  // clear stats table
  statstable stats(_self, _self.value);
  while (stats.begin() != stats.end()) {
    auto itr = stats.end();
    itr--;
    stats.erase(itr);
  }

  // clear old format pair table
  opairstable opairs(_self, _self.value);
  while (opairs.begin() != opairs.end()) {
    auto pitr = opairs.end();
    pitr--;

    statstable pstats(_self, pitr->name.value);
    while (pstats.begin() != pstats.end()) {
      auto sitr = pstats.end();
      sitr--;
      pstats.erase(sitr);
    }

    opairs.erase(pitr);
  }

  // clear new format pair table
  npairstable npairs(_self, _self.value);
  while (npairs.begin() != npairs.end()) {
    auto pitr = npairs.end();
    pitr--;

    statstable pstats(_self, pitr->name.value);
    while (pstats.begin() != pstats.end()) {
      auto sitr = pstats.end();
      sitr--;
      pstats.erase(sitr);
    }

    datapointstable datapoints(_self, pitr->name.value);
    while (datapoints.begin() != datapoints.end()) {
      auto ditr = datapoints.end();
      ditr--;
      datapoints.erase(ditr);
    }

    npairs.erase(pitr);
  }

  // clear snapshot table; required for copydata
  snapshottable snapshot(_self, _self.value);
  while (snapshot.begin() != snapshot.end()) {
    auto snitr = snapshot.end();
    snitr--;
    snapshot.erase(snitr);
  }

}
