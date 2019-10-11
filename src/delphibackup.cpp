#include <delphibackup.hpp>

ACTION delphibackup::copydata() {

	require_auth(_self);

	statstable stats(name("delphioracle"), name("delphioracle").value);
	statstable _stats(_self, _self.value);

	snapshottable snapshot(_self, _self.value);

	check(snapshot.begin() == snapshot.end(), "snapshot already exists; call erasedata first");

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
		print("pairs\n", pitr->name);
		_pairs.emplace(_self, [&](auto& p){
			p.id = pitr->id;
			p.name = pitr->name;
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
		pitr++;
	}

	snapshot.emplace(_self, [&](auto& s) {
		s.id = 0;
		s.timestamp = current_time_point();
	});

}

ACTION delphibackup::erasedata() {

	require_auth(_self);

	statstable stats(_self, _self.value);
  while (stats.begin() != stats.end()) {
    auto itr = stats.end();
    itr--;
    stats.erase(itr);
  }

	pairstable pairs(_self, _self.value);
  while (pairs.begin() != pairs.end()) {
  	auto pitr = pairs.end();
  	pitr--;

		statstable pstats(_self, pitr->name.value);
		auto sitr = pstats.begin();

	  while (pstats.begin() != pstats.end()) {
	    auto itr = pstats.end();
	    itr--;
	    pstats.erase(itr);
	  }

		pairs.erase(pitr);
	}

	snapshottable snapshot(_self, _self.value);
  while (snapshot.begin() != snapshot.end()) {
  	auto sitr = snapshot.end();
  	sitr--;
  }

}
