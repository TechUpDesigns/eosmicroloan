#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

#define ds_time eosio::time_point_sec
#define ds_account eosio::name
#define ds_symbol eosio::symbol
#define ds_asset eosio::asset
#define ds_checksum eosio::checksum256
#define ds_stamp eosio::block_timestamp
#define N(X) name{#X}

using namespace std;
using namespace eosio;

CONTRACT eosmicroloan : public contract {
  public:
    using contract::contract;
      eosmicroloan(eosio::name receiver, eosio::name code, datastream<const char*> ds):contract(receiver, code, ds),
    _loans(receiver, receiver.value)
    {}

    //pseudo code here
    //Handled by the loan officer
    //newLoan

    ACTION newloan(name username, float loan_amount, string memo);

//    ACTION getprice(uint64_t pair_id);
// get the price from the Oracle Tables
    ACTION getprice();

//get the price from  newdex public table
    ACTION getpricedex(uint64_t pair_id);

  //  ACTION getotherloan(uint64_t loanid);

    //ACTION issueloan( name username, asset loan_amount);
    //the borrower
    //payLoan --> reduce balance
    //ACTION payloan(name username, uint64_t loanid, float payment);
    //updateLoan --> helper f(x)
    //static constexpr eosio::name newdexpublic_exchangepair{"newdexpublic.exchangepair"_n};

void payloan(name username, uint64_t loanid, float payment);

    void transfer(
        eosio::name from,
        eosio::name to,
        eosio::asset quantity,
        std::string memo
    );

  private:
    TABLE loans {
      name    username;  //the borrowers username
      uint64_t loanid = 0;
       string  memo; //what is the loan for
       float original_loan_amount;
       float current_balance;  //base the balance on USD converted from EOS
      auto primary_key() const { return loanid; }
    };
//local table internet to our contract
    typedef multi_index<name("loans"), loans> loans_table;
    loans_table _loans;

//code to talk to maurice's table.
name difaccEOSM = "eosmicroloan"_n;  //techupdloans
name difscopeEOSM = "eosmicroloan"_n;
//   https://github.com/EOSIO/eosio.cdt/issues/159
 loans_table _eosmicroloan = loans_table(difaccEOSM , difscopeEOSM.value );

//typedef existing_type new_type_name ;
//oracle rates table is from the EOSDT project:
//https://github.com/equilibrium-eosdt/equilibrium-projects
// https://eosdt.com/
//typedefs dervied from ABI file

typedef name n;
typedef asset a;
typedef symbol c;
typedef time_point_sec t;
typedef uint64_t l;
typedef int i;
typedef uint32_t u;
typedef string s;
//typedef byte b;

TABLE oracle_rates{
a rate;
t update;
a provablecb1a_price;
t provablecb1a_update;
a delphioracle_price;
t delphioracle_update;
a equilibriumdsp_price;
t equilibriumdsp_update;
/*
//Table as defined in EOSDT github source code.

TABLE oracle_rates{
    ds_asset rate;
    ds_time update;
    ds_asset provablecb1a_price;
    ds_time provablecb1a_update;
    ds_asset delphioracle_price;
    ds_time delphioracle_update;
    ds_asset equilibriumdsp_price;
    ds_time equilibriumdsp_update;
    */
uint64_t  primary_key() const  {  return rate.symbol.raw();}
  EOSLIB_SERIALIZE(oracle_rates, (rate)(update)(provablecb1a_price)(provablecb1a_update)(delphioracle_price)(delphioracle_update)(equilibriumdsp_price)(equilibriumdsp_update)    );

};

typedef multi_index<"orarates"_n, oracle_rates> orarates_t;

//structure for ndx_symbol
TABLE ndx_symbol{
  eosio::name contract;
  eosio::symbol sym;
  auto primary_key() const {return contract.value;}
};

//exchangepair from newdexpublic
TABLE exchange_pair {
      uint64_t pair_id = 0;
      uint8_t price_precision;
      uint8_t status;
      ndx_symbol base_symbol;
      ndx_symbol quote_symbol;
      name manager;
      eosio::time_point_sec list_time;
      string pair_symbol;
      double current_price;  //double is float64
      uint64_t base_currency_id;
      uint64_t quote_currency_id;
      uint8_t pair_free;
      uint64_t ext1;
      uint64_t ext2;
      string extstr;
      uint64_t primary_key() const { return pair_id; } //this is the primary key of the table
      EOSLIB_SERIALIZE( exchange_pair, (pair_id)(price_precision)(status)(base_symbol)(quote_symbol)(manager)(list_time)(pair_symbol)(current_price)(base_currency_id)(quote_currency_id)(pair_free)(ext1)(ext2)(extstr) )
};
//define the exchange_pair
typedef multi_index<"exchangepair"_n, exchange_pair> exchangepair_t;

};
