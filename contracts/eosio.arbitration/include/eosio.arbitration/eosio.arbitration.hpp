/**
 * Arbitration Contract Interface
 *
 * @author Craig Branscom, Peter Bue, Ed Silva, Douglas Horn
 * @copyright defined in telos/LICENSE.txt
 */

#pragma once
#include <trail.voting.hpp>
#include <eosiolib/action.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/permission.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/types.h>

using namespace std;
using namespace eosio;

class[[eosio::contract("eosio.arbitration")]] arbitration : public eosio::contract {

public:
  using contract::contract;

  arbitration(name s, name code, datastream<const char *> ds);
  
  ~arbitration();

  [[eosio::action]]
  void setconfig(uint16_t max_elected_arbs, uint32_t election_duration, uint32_t start_election, uint32_t arbitrator_term_length, vector<int64_t> fees);

  #pragma region Enums


  //TODO: describe each enum in README

  enum case_state : uint8_t {
    CASE_SETUP,         // 0
    AWAITING_ARBS,      // 1
    CASE_INVESTIGATION, // 2
    HEARING,            // 3
    DELIBERATION,       // 4
    DECISION,           // 5 NOTE: No more joinders allowed
    ENFORCEMENT,        // 6
    RESOLVED,           // 7
    DISMISSED           // 8 NOTE: Dismissed cases advance and stop here
  };

  enum claim_class : uint8_t {
    UNDECIDED,           // 0
    LOST_KEY_RECOVERY,   // 1
    TRX_REVERSAL,        // 2
    EMERGENCY_INTER,     // 3
    CONTESTED_OWNER,     // 4
    UNEXECUTED_RELIEF,   // 5
    CONTRACT_BREACH,     // 6
    MISUSED_CR_IP,       // 7
    A_TORT,              // 8
    BP_PENALTY_REVERSAL, // 9
    WRONGFUL_ARB_ACT,    // 10
    ACT_EXEC_RELIEF,     // 11
    WP_PROJ_FAILURE,     // 12
    TBNOA_BREACH,        // 13
    MISC                 // 14
  };

  enum arb_status : uint8_t {
    AVAILABLE,   // 0
    UNAVAILABLE, // 1
    INACTIVE,    // 2
    SEAT_EXPIRED // 3
  };

  enum election_status : uint8_t {
    OPEN,   // 0
    PASSED, // 1
    FAILED, // 2
    CLOSED  // 3
  };

  enum lang_code : uint8_t {
    ENGL, //0 English 
    FRCH, //1 French
    GRMN, //2 German
    KREA, //3 Korean
    JAPN, //4 Japanese
    CHNA, //5 Chinese
    SPAN, //6 Spanish
    PGSE, //7 Portuguese
    SWED //8 Swedish
  };


  #pragma endregion Enums

  #pragma region Arb_Elections


  [[eosio::action]]
  void initelection();

  [[eosio::action]]
  void regarb(name nominee, string credentials_link); //NOTE: actually regnominee, currently regarb for nonsense governance reasons

  [[eosio::action]]
  void unregnominee(name nominee);

  [[eosio::action]] //TODO: rename?
  void candaddlead(name nominee, string credentials_link);
  
  [[eosio::action]] //TODO: rename?
  void candrmvlead(name nominee);

  [[eosio::action]] //TODO: need nominee param?
  void endelection(name nominee);

                                            
  #pragma endregion Arb_Elections

  #pragma region Case_Setup

  //NOTE: filing a case doesn't require a respondent
  [[eosio::action]]
  void filecase(name claimant, string claim_link);

  //NOTE: adds subsequent claims to a case
  [[eosio::action]]
  void addclaim(uint64_t case_id, string claim_link, name claimant);

  //NOTE: claims can only be removed by a claimant during case setup
  [[eosio::action]]
  void removeclaim(uint64_t case_id, string claim_hash, name claimant);

  //NOTE: member-level case removal, called during CASE_SETUP
  [[eosio::action]]
  void shredcase(uint64_t case_id, name claimant);

  //NOTE: enforce claimant has at least 1 claim before readying                                 
  [[eosio::action]]
  void readycase(uint64_t case_id, name claimant);


  #pragma endregion Case_Setup

  #pragma region Case_Progression


  [[eosio::action]]
  void assigntocase(uint64_t case_id, name arb_to_assign);

  [[eosio::action]]
  void addarbs(uint64_t case_id, name assigned_arb, uint8_t num_arbs_to_assign);

  [[eosio::action]]
  void dismissclaim(uint64_t case_id, name assigned_arb, string claim_hash, string memo);

  //NOTE: moves to evidence_table and assigns ID
  [[eosio::action]]
  void acceptclaim(uint64_t case_id, name assigned_arb, string claim_hash, string decision_link, uint8_t decision_class);

  [[eosio::action]]
  void advancecase(uint64_t case_id, name assigned_arb);

  //TODO: require rationale?
  [[eosio::action]]
  void dismisscase(uint64_t case_id, name assigned_arb, string ruling_link);

  [[eosio::action]]
  void resolvecase(uint64_t case_id, name assigned_arb, string case_ruling);

  [[eosio::action]]
  void newcfstatus(uint64_t case_id, uint8_t new_status, name assigned_arb);

  [[eosio::action]] //TODO: change rationale to full recusal doc?
  void recuse(uint64_t case_id, string rationale, name assigned_arb);

  [[eosio::action]]
  void newjoinder(uint64_t base_case_id, uint64_t joining_case_id, name arb); //TODO: add memo for joining?

  [[eosio::action]]
  void joincases(uint64_t joinder_id, uint64_t new_case_id, name arb); //TODO: add memo for joining?


  #pragma endregion Case_Progression

  #pragma region Arb_Actions


  [[eosio::action]]
  void newarbstatus(uint8_t new_status, name arbitrator);


  #pragma endregion Arb_Actions

  #pragma region BP_Multisig_Actions


  [[eosio::action]] 
  void dismissarb(name arb);


  #pragma endregion BP_Multisig_Actions

  #pragma region System Structs

  struct permission_level_weight {
    permission_level permission;
    uint16_t weight;

    EOSLIB_SERIALIZE(permission_level_weight, (permission)(weight))
  };

  struct key_weight {
    eosio::public_key key;
    uint16_t weight;

    EOSLIB_SERIALIZE(key_weight, (key)(weight))
  };

  struct wait_weight {
    uint32_t wait_sec;
    uint16_t weight;

    EOSLIB_SERIALIZE(wait_weight, (wait_sec)(weight))
  };

  struct authority {
    uint32_t threshold = 0;
    std::vector<key_weight> keys;
    std::vector<permission_level_weight> accounts;
    std::vector<wait_weight> waits;

    EOSLIB_SERIALIZE(authority, (threshold)(keys)(accounts)(waits))
  };

  #pragma endregion System Structs

  #pragma region Tables and Structs


  /**
   * Holds all arbitrator nominee applications.
   * @scope get_self().value
   * @key uint64_t nominee_name.value
   */
  struct [[eosio::table]] nominee {
    name nominee_name;
    string credentials_link;
    uint32_t application_time;

    uint64_t primary_key() const { return nominee_name.value; }
    EOSLIB_SERIALIZE(nominee, (nominee_name)(credentials_link)(application_time))
  };

  /**
   * Holds all currently elected arbitrators.
   * @scope get_self().value
   * @key uint64_t arb.value
   */
  struct [[eosio::table]] arbitrator {
    name arb;
    uint8_t arb_status;
    vector<uint64_t> open_case_ids;
    vector<uint64_t> closed_case_ids;
    string credentials_link; //NOTE: ipfs_url of arbitrator credentials
    uint32_t elected_time;
    uint32_t term_expiration;
    vector<uint8_t> languages; //NOTE: language codes

    uint64_t primary_key() const { return arb.value; }
    EOSLIB_SERIALIZE(arbitrator, (arb)(arb_status)(open_case_ids)(closed_case_ids)
      (credentials_link)(elected_time)(term_expiration)(languages))
  };

  //NOTE: Stores all information related to a single claim.
  struct [[eosio::table]] claim {
    uint64_t claim_id;
    string claim_summary; //NOTE: ipfs link to claim document
    string decision_link; //NOTE: ipfs link to decision document
    uint8_t decision_class;

    uint64_t primary_key() const { return claim_id; }
    EOSLIB_SERIALIZE(claim, (claim_id)(claim_summary)(decision_link)(decision_class))
  };

  /**
   * Case Files for all arbitration cases.
   * @scope get_self().value
   * @key case_id
   */
  struct [[eosio::table]] casefile {
    uint64_t case_id;
    uint8_t case_status;

    name claimant;
    name respondant; //NOTE: empty for no respondant
    vector<name> arbitrators;
    vector<uint8_t> required_langs;

    vector<claim> unread_claims; //TODO: make vector of claims? don't need to save claims to table unless accepted
    vector<uint64_t> accepted_claims;
    string case_ruling; //NOTE: ipfs hash to ruling doc
    
    string arb_comment;
    uint32_t last_edit;

    uint64_t primary_key() const { return case_id; }

	uint64_t by_claimant() const { return claimant.value; }
	uint128_t by_uuid() const {
		uint128_t claimant_id = static_cast<uint128_t>(claimant.value);
		uint128_t respondant_id = static_cast<uint128_t>(respondant.value);
		return (claimant_id << 64) | respondant_id;
	}
    EOSLIB_SERIALIZE(casefile, (case_id)(case_status)
      (claimant)(respondant)(arbitrators)(required_langs)
      (unread_claims)(accepted_claims)(case_ruling)
      (arb_comment)(last_edit))
  };

  /**
   * Singleton for global config settings.
   * @scope singleton scope (get_self().value)
   * @key table name
   */
  //TODO: make fee structure a constant?
  //NOTE: diminishing subsequent response (default) times
  //NOTE: initial deposit saved
  struct [[ eosio::table ]] config {
    name publisher;
    uint16_t max_elected_arbs;
    uint32_t election_duration;
    uint32_t election_start;
	vector<int64_t> fee_structure; //NOTE: always in TLOS so only store asset.amount value //TODO: just make vector of assets
    uint32_t arb_term_length;
	uint32_t last_time_edited;
    uint64_t current_ballot_id = 0;
	bool auto_start_election = false;

    uint64_t primary_key() const { return publisher.value; }
    EOSLIB_SERIALIZE(config, (publisher)
    (max_elected_arbs)(election_duration)(election_start)(fee_structure)
	(arb_term_length)(last_time_edited)(current_ballot_id)(auto_start_election))
  };

  /**
   * Holds instances of joinder cases.
   * @scope get_self().value
   * @key uint64_t join_id
   */
  struct [[eosio::table]] joinder {
    uint64_t join_id;
    vector<uint64_t> cases;
    uint32_t join_time;
    name joined_by;

    uint64_t primary_key() const { return join_id; }
    EOSLIB_SERIALIZE(joinder, (join_id)(cases)(join_time)(joined_by))
  };

protected:

  typedef multi_index<name("nominees"), nominee> nominees_table;

  typedef multi_index<name("arbitrators"), arbitrator> arbitrators_table;

  typedef multi_index<name("casefiles"), casefile> casefiles_table;

  typedef multi_index<name("joinedcases"), joinder> joinders_table;

  typedef multi_index<name("claims"), claim> claims_table;

  typedef singleton<name("config"), config> config_singleton;
  config_singleton configs;
  config _config;
   
  #pragma endregion Tables and Structs

  #pragma region Helpers

  void validate_ipfs_url(string ipfs_url);
  
  config get_default_config();
  
  void start_new_election(uint8_t available_seats);
  
  bool has_available_seats(arbitrators_table &arbitrators, uint8_t &available_seats);

  void add_arbitrator(arbitrators_table &arbitrators, name arb_name, std::string credential_link);

  void assert_string(string to_check, string error_msg);

  vector<claim>::iterator get_claim_at(const string claim_hash, vector<claim> claims);

  void del_claim_at(const string claim_hash, vector<claim> claims);

  template <typename T, typename func>
  void map(vector<T>& arr, func&& handler) {
	  for(auto it = arr.begin(); it != arr.end(); ++it){
		  handler(it);
	  }	
  }

  #pragma endregion Helpers

};
