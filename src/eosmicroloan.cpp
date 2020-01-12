#include <eosmicroloan.hpp>
//#include <eos_api.hpp>
#define EOS_SYMBOL symbol("EOS", 4)
#include <eosio/print.hpp>
//#include <json.hpp>
#include <cmath>
///using nlohmann::json;

#define USD_SYMBOL symbol("USD",4)


bool verifyAsset(asset token) {
auto sym = token.symbol;

check( sym.is_valid(), "invalid token");
check( token.symbol == EOS_SYMBOL, "only EOS tokens");
check( token.amount > 0, "only positive quantity allowed");

return true;

}

//ACTION eosmicroloan::getprice(uint64_t pair_id){
ACTION eosmicroloan::getprice(){

print("getting prices...");
name difacc = "eosdtorclize"_n;  
name difscope = "eosdtorclize"_n;

//create variable oracle_rates at time the
orarates_t oracle_rates(difacc, difscope.value);
        auto itr = oracle_rates.find(USD_SYMBOL.raw());
        check(itr != oracle_rates.end(), "oracle rate % not found.", USD_SYMBOL.raw());
        print(itr->rate);

for(auto& myPrice : oracle_rates) {
    //check(myPrice != _newdexpublic.end() , "Table is empty");
    auto myRate = (myPrice.rate);
    print(myRate);
  } 

}


ACTION eosmicroloan::getpricedex(uint64_t pair_id){
  //require_auth(_self);  //require contract 
 
print("getting prices...");
name difacc = "newdexpublic"_n;  
name difscope = "newdexpublic"_n;

exchangepair_t _newdexpublic(difacc, difscope.value);

  auto myPrice = _newdexpublic.find(pair_id);
        check(myPrice != _newdexpublic.end(), "newdexpublic pair_id % not found.", pair_id);
        print(myPrice->current_price);

}
/* ************************

ACTION eosmicroloan::getotherloan(uint64_t loanid){
  require_auth(_self);  //require contract 
  auto myLoan = _eosmicroloan.find(loanid);
  check(myLoan != _eosmicroloan.end(), "Loan does not exist");

  print("here are the results of getotherloan: ");
  print (myLoan->memo);

}
*/

ACTION eosmicroloan::newloan(name username, float loan_amount, string memo) {
  require_auth(_self);  //require contract to make the loan

//loans_table _loans;
//update the table to include a new loan
//the contract pays for the entry

//verify is a real user

check( is_account( username ), "borrower account does not exist");
//verify symbol is valid + verify a symbol was given
//verifyAsset(loan_amount);

//verify loans are positive
check(loan_amount>0,"invalid loan amount"); 
  // update the table to include a new newloan
    _loans.emplace(_self, [&](auto& newloan) {
      newloan.loanid = _loans.available_primary_key();
        newloan.original_loan_amount = loan_amount;
        newloan.current_balance = loan_amount;
        newloan.memo = memo;
        newloan.username = username;
    
    });
 
};

  //ACTION eosmicroloan::issueloan( name username, asset loan_amount){}
    //the borrower
    //payLoan --> reduce balance
  
/****************               */
//ACTION eosmicroloan::payloan(name username, uint64_t loanid, float payment){
void eosmicroloan::payloan(name username, uint64_t loanid, float payment){

  require_auth(username);
  auto myLoan = _loans.find(loanid);
  //check(verifyAsset(payment), "cannot verify  the payment type");
  //make sure the loan exists
  check(myLoan != _loans.end(), "Loan does not exist");

  //make sure the loan matches the username
  check(myLoan->username == username, "Loans do  not match");
  
  _loans.modify(myLoan, _self, [&]( auto& row ) {
      row.current_balance = myLoan->current_balance - payment;
});

};
 

void eosmicroloan::transfer(name from, name to, asset payment, string memo) {

  if (from == _self) {
  //we're sending money, do nothing additional
    return;
  }
  //validate the transfer paramters
  check(to == _self, "contract is not involved in this transfer");
  //check(payment.symbol.is_valid(), "invalid payment");
  //check(payment.amount > 0, "only positive payment allowed");
  //check(payment.symbol == EOS_SYMBOL, "only EOS tokens allowed");
  
  //validate the quantity with our verifier
  check(verifyAsset(payment), "invalid token");

  //init the loanid to zero
  uint64_t loanid = -1;
  float current_balance;
  //loop through our loans table for the user
  for(auto& item : _loans) {
    if (item.username == from && item.current_balance > 0) {
        loanid = item.loanid;
        current_balance = item.current_balance;
    }
  }

// https://eosio.stackexchange.com/questions/5086/convert-string-to-double-flaot32-float64-in-eosio
  string in = memo;
  std::size_t input_dot_index = in.find(".");

  int64_t input_digits = in.length() - input_dot_index - 1;

   in.erase(input_dot_index, 1);
   double conversionFactor = atoi(in.c_str()) / pow(10, input_digits);


  //check for overpayment
  //should do the conversion here.
  //auto j = json::parse(memo);
  //auto conversionFactor = j->cf;
  //auto conversionFactor = stringtodouble(memo); //j["cf"];
//eos is  2.64 dollars/EOS
//user sent   1.2 EOS
//therefore dollars =     CF * EOS
check(conversionFactor > 0.0001, "invalid conversion");
float dollarPayment = conversionFactor * (payment.amount/10000.0);
check(current_balance >= dollarPayment, "no overpayments:  "+ to_string(payment.amount));
//print(payment.amount);
//additional security checks here
//string jKEY = j["key"];  //does nothing yet. NYI
  //check(current_balance >= payment.amount, "no overpayments");
//in that case payment should be a float.

  if(loanid >= 0){
  //loan payment has been found. payloan
  payloan(from, loanid, dollarPayment);
  }

  return; //we're receiving tokens?
}
extern "C" void apply(uint64_t receiver, uint64_t code, uint64_t action)
{
  if (code == "eosio.token"_n.value && action && action == "transfer"_n.value){
    eosio::execute_action(
      eosio::name(receiver), eosio::name(code), &eosmicroloan::transfer
    );
  }
  else if (code == receiver) {
    switch(action) {
//      EOSIO_DISPATCH_HELPER(eosmicroloan, (newloan)(payloan)(getotherloan)(getprice) )
      EOSIO_DISPATCH_HELPER(eosmicroloan, (newloan)(getprice) (getpricedex))
    }
  }
}

//EOSIO_DISPATCH(eosmicroloan, (newloan)(payloan))
