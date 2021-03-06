#pragma once
#include <steemit/chain/protocol/base.hpp>
#include <steemit/chain/protocol/block_header.hpp>
#include <steemit/chain/protocol/asset.hpp>

#include <fc/utf8.hpp>

namespace steemit { namespace chain {

   struct account_create_operation : public base_operation
   {
      asset             fee;
      string            creator;
      string            new_account_name;
      authority         owner;
      authority         active;
      authority         posting;
      public_key_type   memo_key;
      string            json_metadata;

      void validate()const;
      void get_required_active_authorities( flat_set<string>& a )const{ a.insert(creator); }
   };

   struct account_update_operation : public base_operation
   {
      string                        account;
      optional< authority >         owner;
      optional< authority >         active;
      optional< authority >         posting;
      public_key_type               memo_key;
      string                        json_metadata;

      void validate()const;

      void get_required_owner_authorities( flat_set<string>& a )const
      { if( owner ) a.insert( account ); }

      void get_required_active_authorities( flat_set<string>& a )const
      { if( !owner /*&& active*/) a.insert( account ); }

      //void get_required_posting_authorities( flat_set<string>& a )const
      //{ if( !active && !owner && posting ) a.insert( account ); }
   };

   struct comment_operation : public base_operation
   {
      string            parent_author;
      string            parent_permlink;

      string            author;
      string            permlink;

      string            title;
      string            body;
      string            json_metadata;

      void validate()const;
      void get_required_posting_authorities( flat_set<string>& a )const{ a.insert(author); }
   };

   /**
    *  Authors of posts may not want all of the benefits that come from creating a post. This
    *  operation allows authors to update properties associated with their post.
    *
    *  The max_accepted_payout may be decreased, but never increased.
    *  The percent_steem_dollars may be decreased, but never increased
    *
    */
   struct comment_options_operation : public base_operation
   {
      string author;
      string permlink;

      asset    max_accepted_payout    = asset( 1000000000, SBD_SYMBOL );       /// SBD value of the maximum payout this post will receive
      uint16_t percent_steem_dollars  = STEEMIT_100_PERCENT; /// the percent of Steem Dollars to key, unkept amounts will be received as Steem Power
      bool     allow_votes            = true;      /// allows a post to receive votes;
      bool     allow_curation_rewards = true; /// allows voters to recieve curation rewards. Rewards return to reward fund.
      extensions_type extensions;

      void validate()const;
      void get_required_posting_authorities( flat_set<string>& a )const{ a.insert(author); }
   };

   struct challenge_authority_operation : public base_operation
   {
      string challenger;
      string challenged;
      bool   require_owner = false;

      void validate()const;

      void get_required_active_authorities( flat_set<string>& a )const{ a.insert(challenger); }
   };

   struct prove_authority_operation : public base_operation
   {
      string challenged;
      bool   require_owner = false;

      void validate()const;

      void get_required_active_authorities( flat_set<string>& a )const{ if( !require_owner ) a.insert(challenged); }
      void get_required_owner_authorities( flat_set<string>& a )const{  if(  require_owner ) a.insert(challenged); }
   };

   struct delete_comment_operation : public base_operation
   {
      string author;
      string permlink;

      void validate()const;
      void get_required_posting_authorities( flat_set<string>& a )const{ a.insert(author); }
   };

   struct vote_operation : public base_operation
   {
      string    voter;
      string    author;
      string    permlink;
      int16_t   weight = 0;

      void validate()const;
      void get_required_posting_authorities( flat_set<string>& a )const{ a.insert(voter); }
   };

   struct comment_reward_operation : public base_operation
   {
      comment_reward_operation(){}
      comment_reward_operation( const string& a, const string& p, const asset& s, const asset& v )
         :author(a),permlink(p),sbd_payout(s),vesting_payout(v){}
      string author;
      string permlink;
      asset  sbd_payout;
      asset  vesting_payout;
      void  validate()const { FC_ASSERT( false, "this is a virtual operation" ); }
   };

   struct curate_reward_operation : public base_operation
   {
      curate_reward_operation(){}
      curate_reward_operation( const string& c, const asset& r, const string& a, const string& p )
         :curator(c),reward(r),comment_author(a),comment_permlink(p){}

      string curator;
      asset  reward;
      string comment_author;
      string comment_permlink;
      void   validate()const { FC_ASSERT( false, "this is a virtual operation" ); }
   };

   struct comment_payout_operation : public base_operation
   {
      comment_payout_operation(){}
      comment_payout_operation( const string& a, const string& pl, const asset& p )
         :author(a),permlink(pl),payout(p){}

      string author;
      string permlink;
      asset  payout;
      void   validate()const { FC_ASSERT( false, "this is a virtual operation" ); }
   };

   struct liquidity_reward_operation : public base_operation
   {
      liquidity_reward_operation( string o = string(), asset p = asset() )
      :owner(o),payout(p){}

      string owner;
      asset  payout;
      void  validate()const { FC_ASSERT( false, "this is a virtual operation" ); }
   };

   struct interest_operation : public base_operation
   {
      interest_operation( const string& o = "", const asset& i = asset(0,SBD_SYMBOL) )
         :owner(o),interest(i){}

      string owner;
      asset  interest;

      void  validate()const { FC_ASSERT( false, "this is a virtual operation" ); }
   };

   struct fill_convert_request_operation : public base_operation
   {
      fill_convert_request_operation(){}
      fill_convert_request_operation( const string& o, const uint32_t id, const asset& in, const asset& out )
         :owner(o), requestid(id), amount_in(in), amount_out(out){}
      string   owner;
      uint32_t requestid = 0;
      asset    amount_in;
      asset    amount_out;
      void  validate()const { FC_ASSERT( false, "this is a virtual operation" ); }
   };

   struct fill_vesting_withdraw_operation : public base_operation
   {
      fill_vesting_withdraw_operation(){}
      fill_vesting_withdraw_operation( const string& f, const string& t, const asset& w, const asset& d )
         :from_account(f), to_account(t), withdrawn(w), deposited(d){}
      string from_account;
      string to_account;
      asset  withdrawn;
      asset  deposited;

      void  validate()const { FC_ASSERT( false, "this is a virtual operation" ); }
   };

   /**
    * @ingroup operations
    *
    * @brief Transfers STEEM from one account to another.
    */
   struct transfer_operation : public base_operation
   {
      string            from;
      /// Account to transfer asset to
      string            to;
      /// The amount of asset to transfer from @ref from to @ref to
      asset             amount;

      /// The memo is plain-text, any encryption on the memo is up to
      /// a higher level protocol.
      string            memo;

      void              validate()const;
      void get_required_active_authorities( flat_set<string>& a )const{ if(amount.symbol != VESTS_SYMBOL) a.insert(from); }
      void get_required_owner_authorities( flat_set<string>& a )const { if(amount.symbol == VESTS_SYMBOL) a.insert(from); }
   };

   /**
    *  The purpose of this operation is to enable someone to send money contingently to
    *  another individual. The funds leave the *from* account and go into a temporary balance
    *  where they are held until *from* releases it to *to*   or *to* refunds it to *from*.
    *
    *  In the event of a dispute the *agent* can divide the funds between the to/from account.
    *
    *  The escrow agent is paid the fee no matter what. It is up to the escrow agent to determine
    *
    *  Escrow transactions are uniquely identified by 'from' and 'escrow_id', the 'escrow_id' is defined
    *  by the sender.
    */
   struct escrow_transfer_operation : public base_operation
   {
      string         from;
      string         to;
      asset          amount;
      string         memo;

      uint32_t        escrow_id;
      string         agent;
      asset          fee;
      string         json_meta;
      time_point_sec expiration;

      void validate()const;
      void get_required_active_authorities( flat_set<string>& a )const{ a.insert(from); }
   };

   /**
    *  If either the sender or receiver of an escrow payment has an issue, they can
    *  raise it for dispute. Once a payment is in dispute, the agent has authority over
    *  who gets what.
    */
   struct escrow_dispute_operation : public base_operation
   {
      string   from;
      string   to;
      uint32_t escrow_id;
      string   who;

      void validate()const;
      void get_required_active_authorities( flat_set<string>& a )const{ a.insert(who); }
   };

   /**
    *  This operation can be used by anyone associated with the escrow transfer to
    *  release funds if they have permission.
    */
   struct escrow_release_operation : public base_operation
   {
      string    from;
      uint32_t  escrow_id;
      string    to; ///< the account that should receive funds (might be from, might be to
      string    who; ///< the account that is attempting to release the funds, determines valid 'to'
      asset     amount; ///< the amount of funds to release

      void validate()const;
      void get_required_active_authorities( flat_set<string>& a )const{ a.insert(who); }
   };


   /**
    *  This operation converts STEEM into VFS (Vesting Fund Shares) at
    *  the current exchange rate. With this operation it is possible to
    *  give another account vesting shares so that faucets can
    *  pre-fund new accounts with vesting shares.
    */
   struct transfer_to_vesting_operation : public base_operation
   {
      string   from;
      string   to; ///< if null, then same as from
      asset    amount; ///< must be STEEM


      void validate()const;
      void get_required_active_authorities( flat_set<string>& a )const{ a.insert(from); }
   };

   /**
    * At any given point in time an account can be withdrawing from their
    * vesting shares. A user may change the number of shares they wish to
    * cash out at any time between 0 and their total vesting stake.
    *
    * After applying this operation, vesting_shares will be withdrawn
    * at a rate of vesting_shares/104 per week for two years starting
    * one week after this operation is included in the blockchain.
    *
    * This operation is not valid if the user has no vesting shares.
    */
   struct withdraw_vesting_operation : public base_operation
   {
      string      account;
      asset       vesting_shares;

      void validate()const;
      void get_required_active_authorities( flat_set<string>& a )const{ a.insert(account); }
   };

   /**
    * Allows an account to setup a vesting withdraw but with the additional
    * request for the funds to be transferred directly to another account's
    * balance rather than the withdrawing account. In addition, those funds
    * can be immediately vested again, circumventing the conversion from
    * vests to steem and back, guaranteeing they maintain their value.
    */
   struct set_withdraw_vesting_route_operation : public base_operation
   {
      string   from_account;
      string   to_account;
      uint16_t percent;
      bool     auto_vest;

      void validate()const;
      void get_required_active_authorities( flat_set< string >& a )const { a.insert( from_account ); }
   };

   /**
    * Witnesses must vote on how to set certain chain properties to ensure a smooth
    * and well functioning network.  Any time @owner is in the active set of witnesses these
    * properties will be used to control the blockchain configuration.
    */
   struct chain_properties
   {
      /**
       *  This fee, paid in STEEM, is converted into VESTING SHARES for the new account. Accounts
       *  without vesting shares cannot earn usage rations and therefore are powerless. This minimum
       *  fee requires all accounts to have some kind of commitment to the network that includes the
       *  ability to vote and make transactions.
       */
      asset             account_creation_fee =
         asset( STEEMIT_MIN_ACCOUNT_CREATION_FEE, STEEM_SYMBOL );

      /**
       *  This witnesses vote for the maximum_block_size which is used by the network
       *  to tune rate limiting and capacity
       */
      uint32_t          maximum_block_size = STEEMIT_MIN_BLOCK_SIZE_LIMIT * 2;
      uint16_t          sbd_interest_rate  = STEEMIT_DEFAULT_SBD_INTEREST_RATE;

      void validate()const {
         FC_ASSERT( account_creation_fee.amount >= STEEMIT_MIN_ACCOUNT_CREATION_FEE);
         FC_ASSERT( maximum_block_size >= STEEMIT_MIN_BLOCK_SIZE_LIMIT);
         FC_ASSERT( sbd_interest_rate >= 0 );
         FC_ASSERT( sbd_interest_rate <= STEEMIT_100_PERCENT );
      }
   };

   /**
    *  Users who wish to become a witness must pay a fee acceptable to
    *  the current witnesses to apply for the position and allow voting
    *  to begin.
    *
    *  If the owner isn't a witness they will become a witness.  Witnesses
    *  are charged a fee equal to 1 weeks worth of witness pay which in
    *  turn is derived from the current share supply.  The fee is
    *  only applied if the owner is not already a witness.
    *
    *  If the block_signing_key is null then the witness is removed from
    *  contention.  The network will pick the top 21 witnesses for
    *  producing blocks.
    */
   struct witness_update_operation : public base_operation
   {
      string            owner;
      string            url;
      public_key_type   block_signing_key;
      chain_properties  props;
      asset             fee; ///< the fee paid to register a new witness, should be 10x current block production pay

      void validate()const;
      void get_required_active_authorities( flat_set<string>& a )const{ a.insert(owner); }
   };

   /**
    * All accounts with a VFS can vote for or against any witness.
    *
    * If a proxy is specified then all existing votes are removed.
    */
   struct account_witness_vote_operation : public base_operation
   {
      string  account;
      string  witness;
      bool    approve = true;

      void validate() const;
      void get_required_active_authorities( flat_set<string>& a )const{ a.insert(account); }
   };

   struct account_witness_proxy_operation : public base_operation
   {
      string  account;
      string  proxy;

      void validate()const;
      void get_required_active_authorities( flat_set<string>& a )const{ a.insert(account); }
   };

   /**
    * @brief provides a generic way to add higher level protocols on top of witness consensus
    * @ingroup operations
    *
    * There is no validation for this operation other than that required auths are valid
    */
   struct custom_operation : public base_operation
   {
      flat_set<string>  required_auths;
      uint16_t          id = 0;
      vector<char>      data;

      void validate()const;
      void get_required_active_authorities( flat_set<string>& a )const{ for( const auto& i : required_auths ) a.insert(i); }
   };

   /** serves the same purpose as custom_operation but also supports required posting authorities. Unlike custom_operation,
    * this operation is designed to be human readable/developer friendly.
    **/
   struct custom_json_operation : public base_operation
   {
      flat_set<string>  required_auths;
      flat_set<string>  required_posting_auths;
      string            id; ///< must be less than 32 characters long
      string            json; ///< must be proper utf8 / JSON string.

      void validate()const;
      void get_required_active_authorities( flat_set<string>& a )const{ for( const auto& i : required_auths ) a.insert(i); }
      void get_required_posting_authorities( flat_set<string>& a )const{ for( const auto& i : required_posting_auths ) a.insert(i); }
   };

   /**
    *  Feeds can only be published by the top N witnesses which are included in every round and are
    *  used to define the exchange rate between steem and the dollar.
    */
   struct feed_publish_operation : public base_operation
   {
      string   publisher;
      price    exchange_rate;

      void  validate()const;
      void  get_required_active_authorities( flat_set<string>& a )const{ a.insert(publisher); }
   };

   /**
    *  This operation instructs the blockchain to start a conversion between STEEM and SBD,
    *  The funds are deposited after STEEMIT_CONVERSION_DELAY
    */
   struct convert_operation : public base_operation
   {
      string   owner;
      uint32_t requestid = 0;
      asset    amount;

      void  validate()const;
      void  get_required_active_authorities( flat_set<string>& a )const{ a.insert(owner); }
   };

   /**
    * This operation creates a limit order and matches it against existing open orders.
    */
   struct limit_order_create_operation : public base_operation
   {
      string           owner;
      uint32_t         orderid = 0; /// an ID assigned by owner, must be unique
      asset            amount_to_sell;
      asset            min_to_receive;
      bool             fill_or_kill = false;
      time_point_sec   expiration = time_point_sec::maximum();

      void  validate()const;
      void  get_required_active_authorities( flat_set<string>& a )const{ a.insert(owner); }

      price           get_price()const { return amount_to_sell / min_to_receive; }

      pair<asset_symbol_type,asset_symbol_type> get_market()const
      {
         return amount_to_sell.symbol < min_to_receive.symbol ?
                std::make_pair(amount_to_sell.symbol, min_to_receive.symbol) :
                std::make_pair(min_to_receive.symbol, amount_to_sell.symbol);
      }
   };

   /**
    *  This operation is identical to limit_order_create except it serializes the price rather
    *  than calculating it from other fields.
    */
   struct limit_order_create2_operation : public base_operation
   {
      string           owner;
      uint32_t         orderid = 0; /// an ID assigned by owner, must be unique
      asset            amount_to_sell;
      bool             fill_or_kill = false;
      price            exchange_rate;
      time_point_sec   expiration = time_point_sec::maximum();

      void  validate()const;
      void  get_required_active_authorities( flat_set<string>& a )const{ a.insert(owner); }

      price           get_price()const { return exchange_rate; }

      pair<asset_symbol_type,asset_symbol_type> get_market()const
      {
         return exchange_rate.base.symbol < exchange_rate.quote.symbol ?
                std::make_pair(exchange_rate.base.symbol, exchange_rate.quote.symbol) :
                std::make_pair(exchange_rate.quote.symbol, exchange_rate.base.symbol);
      }
   };

   struct fill_order_operation : public base_operation
   {
      fill_order_operation(){}
      fill_order_operation( const string& c_o, uint32_t c_id, const asset& c_p, const string& o_o, uint32_t o_id, const asset& o_p )
      :current_owner(c_o),current_orderid(c_id),current_pays(c_p),open_owner(o_o),open_orderid(o_id),open_pays(o_p){}

      string   current_owner;
      uint32_t current_orderid;
      asset    current_pays;
      string   open_owner;
      uint32_t open_orderid;
      asset    open_pays;
      void  validate()const { FC_ASSERT( false, "this is a virtual operation" ); }
   };

   /**
    *  Cancels an order and returns the balance to owner.
    */
   struct limit_order_cancel_operation : public base_operation
   {
      string   owner;
      uint32_t orderid = 0;

      void  validate()const;
      void  get_required_active_authorities( flat_set<string>& a )const{ a.insert(owner); }
   };

   struct pow
   {
      public_key_type   worker;
      digest_type       input;
      signature_type    signature;
      digest_type       work;

      void create( const fc::ecc::private_key& w, const digest_type& i );
      void validate()const;
   };

   struct pow_operation : public base_operation
   {
      string           worker_account;
      block_id_type    block_id;
      uint64_t         nonce = 0;
      pow              work;
      chain_properties props;

      void validate()const;
      fc::sha256 work_input()const;

      /** there is no need to verify authority, the proof of work is sufficient */
      void get_required_active_authorities( flat_set<string>& a )const{  }
   };

   /**
    * This operation is used to report a miner who signs two blocks
    * at the same time. To be valid, the violation must be reported within
    * STEEMIT_MAX_WITNESSES blocks of the head block (1 round) and the
    * producer must be in the ACTIVE witness set.
    *
    * Users not in the ACTIVE witness set should not have to worry about their
    * key getting compromised and being used to produced multiple blocks so
    * the attacker can report it and steel their vesting steem.
    *
    * The result of the operation is to transfer the full VESTING STEEM balance
    * of the block producer to the reporter.
    */
   struct report_over_production_operation : public base_operation
   {
      string              reporter;
      signed_block_header first_block;
      signed_block_header second_block;

      void validate()const;
   };

   /**
    * All account recovery requests come from a listed recovery account. This
    * is secure based on the assumption that only a trusted account should be
    * a recovery account. It is the responsibility of the recovery account to
    * verify the identity of the account holder of the account to recover by
    * whichever means they have agreed upon. The blockchain assumes identity
    * has been verified when this operation is broadcast.
    *
    * This operation creates an account recovery request which the account to
    * recover has 24 hours to respond to before the request expires and is
    * invalidated.
    *
    * There can only be one active recovery request per account at any one time.
    * Pushing this operation for an account to recover when it already has
    * an active request will either update the request to a new new owner authority
    * and extend the request expiration to 24 hours from the current head block
    * time or it will delete the request. To cancel a request, simply set the
    * weight threshold of the new owner authority to 0, making it an open authority.
    *
    * Additionally, the new owner authority must be satisfiable. In other words,
    * the sum of the key weights must be greater than or equal to the weight
    * threshold.
    *
    * This operation only needs to be signed by the the recovery account.
    * The account to recover confirms its identity to the blockchain in
    * the recover account operation.
    */
   struct request_account_recovery_operation : public base_operation
   {
      string            recovery_account;       ///< The recovery account is listed as the recovery account on the account to recover.

      string            account_to_recover;     ///< The account to recover. This is likely due to a compromised owner authority.

      authority         new_owner_authority;    ///< The new owner authority the account to recover wishes to have. This is secret
                                                ///< known by the account to recover and will be confirmed in a recover_account_operation

      extensions_type   extensions;             ///< Extensions. Not currently used.

      void get_required_active_authorities( flat_set<string>& a )const{ a.insert( recovery_account ); }

      void validate() const;
   };

   /**
    * Recover an account to a new authority using a previous authority and verification
    * of the recovery account as proof of identity. This operation can only succeed
    * if there was a recovery request sent by the account's recover account.
    *
    * In order to recover the account, the account holder must provide proof
    * of past ownership and proof of identity to the recovery account. Being able
    * to satisfy an owner authority that was used in the past 30 days is sufficient
    * to prove past ownership. The get_owner_history function in the database API
    * returns past owner authorities that are valid for account recovery.
    *
    * Proving identity is an off chain contract between the account holder and
    * the recovery account. The recovery request contains a new authority which
    * must be satisfied by the account holder to regain control. The actual process
    * of verifying authority may become complicated, but that is an application
    * level concern, not a blockchain concern.
    *
    * This operation requires both the past and future owner authorities in the
    * operation because neither of them can be derived from the current chain state.
    * The operation must be signed by keys that satisfy both the new owner authority
    * and the recent owner authority. Failing either fails the operation entirely.
    *
    * If a recovery request was made inadvertantly, the account holder should
    * contact the recovery account to have the request deleted.
    *
    * The two setp combination of the account recovery request and recover is
    * safe because the recovery account never has access to secrets of the account
    * to recover. They simply act as an on chain endorsement of off chain identity.
    * In other systems, a fork would be required to enforce such off chain state.
    * Additionally, an account cannot be permanently recovered to the wrong account.
    * While any owner authority from the past 30 days can be used, including a compromised
    * authority, the account can be continually recovered until the recovery account
    * is confident a combination of uncompromised authorities were used to
    * recover the account. The actual process of verifying authority may become
    * complicated, but that is an application level concern, not the blockchain's
    * concern.
    */
   struct recover_account_operation : public base_operation
   {
      string            account_to_recover;        ///< The account to be recovered

      authority         new_owner_authority;       ///< The new owner authority as specified in the request account recovery operation.

      authority         recent_owner_authority;    ///< A previous owner authority that the account holder will use to prove past ownership of the account to be recovered.

      extensions_type   extensions;                ///< Extensions. Not currently used.

      void get_required_authorities( vector<authority>& a )const
      {
         a.push_back( new_owner_authority );
         a.push_back( recent_owner_authority );
      }

      void validate() const;
   };

   /**
    * Each account lists another account as their recovery account.
    * The recovery account has the ability to create account_recovery_requests
    * for the account to recover. An account can change their recovery account
    * at any time with a 30 day delay. This delay is to prevent
    * an attacker from changing the recovery account to a malicious account
    * during an attack. These 30 days match the 30 days that an
    * owner authority is valid for recovery purposes.
    *
    * On account creation the recovery account is set either to the creator of
    * the account (The account that pays the creation fee and is a signer on the transaction)
    * or to the empty string if the account was mined. An account with no recovery
    * has the top voted witness as a recovery account, at the time the recover
    * request is created. Note: This does mean the effective recovery account
    * of an account with no listed recovery account can change at any time as
    * witness vote weights. The top voted witness is explicitly the most trusted
    * witness according to stake.
    */
   struct change_recovery_account_operation : public base_operation
   {
      string            account_to_recover;     ///< The account that would be recovered in case of compromise

      string            new_recovery_account;   ///< The account that creates the recover request

      extensions_type   extensions;             ///< Extensions. Not currently used.

      void get_required_owner_authorities( flat_set<string>& a )const{ a.insert( account_to_recover ); }

      void validate() const;
   };
} } // steemit::chain

FC_REFLECT( steemit::chain::report_over_production_operation, (reporter)(first_block)(second_block) )
FC_REFLECT( steemit::chain::convert_operation, (owner)(requestid)(amount) )
FC_REFLECT( steemit::chain::feed_publish_operation, (publisher)(exchange_rate) )
FC_REFLECT( steemit::chain::pow, (worker)(input)(signature)(work) )
FC_REFLECT( steemit::chain::chain_properties, (account_creation_fee)(maximum_block_size)(sbd_interest_rate) );

FC_REFLECT( steemit::chain::pow_operation, (worker_account)(block_id)(nonce)(work)(props) )

FC_REFLECT( steemit::chain::account_create_operation,
            (fee)
            (creator)
            (new_account_name)
            (owner)
            (active)
            (posting)
            (memo_key)
            (json_metadata) )

FC_REFLECT( steemit::chain::account_update_operation,
            (account)
            (owner)
            (active)
            (posting)
            (memo_key)
            (json_metadata) )

FC_REFLECT( steemit::chain::transfer_operation, (from)(to)(amount)(memo) )
FC_REFLECT( steemit::chain::transfer_to_vesting_operation, (from)(to)(amount) )
FC_REFLECT( steemit::chain::withdraw_vesting_operation, (account)(vesting_shares) )
FC_REFLECT( steemit::chain::set_withdraw_vesting_route_operation, (from_account)(to_account)(percent)(auto_vest) )
FC_REFLECT( steemit::chain::witness_update_operation, (owner)(url)(block_signing_key)(props)(fee) )
FC_REFLECT( steemit::chain::account_witness_vote_operation, (account)(witness)(approve) )
FC_REFLECT( steemit::chain::account_witness_proxy_operation, (account)(proxy) )
FC_REFLECT( steemit::chain::comment_operation, (parent_author)(parent_permlink)(author)(permlink)(title)(body)(json_metadata) )
FC_REFLECT( steemit::chain::vote_operation, (voter)(author)(permlink)(weight) )
FC_REFLECT( steemit::chain::custom_operation, (required_auths)(id)(data) )
FC_REFLECT( steemit::chain::custom_json_operation, (required_auths)(required_posting_auths)(id)(json) )
FC_REFLECT( steemit::chain::limit_order_create_operation, (owner)(orderid)(amount_to_sell)(min_to_receive)(fill_or_kill)(expiration) )
FC_REFLECT( steemit::chain::limit_order_create2_operation, (owner)(orderid)(amount_to_sell)(exchange_rate)(fill_or_kill)(expiration) )
FC_REFLECT( steemit::chain::fill_order_operation, (current_owner)(current_orderid)(current_pays)(open_owner)(open_orderid)(open_pays) );
FC_REFLECT( steemit::chain::limit_order_cancel_operation, (owner)(orderid) )

FC_REFLECT( steemit::chain::comment_reward_operation, (author)(permlink)(sbd_payout)(vesting_payout) )
FC_REFLECT( steemit::chain::curate_reward_operation, (curator)(reward)(comment_author)(comment_permlink) )
FC_REFLECT( steemit::chain::comment_payout_operation, (author)(permlink)(payout) )
FC_REFLECT( steemit::chain::fill_convert_request_operation, (owner)(requestid)(amount_in)(amount_out) )
FC_REFLECT( steemit::chain::liquidity_reward_operation, (owner)(payout) )
FC_REFLECT( steemit::chain::interest_operation, (owner)(interest) )
FC_REFLECT( steemit::chain::fill_vesting_withdraw_operation, (from_account)(to_account)(withdrawn)(deposited) )
FC_REFLECT( steemit::chain::delete_comment_operation, (author)(permlink) );
FC_REFLECT( steemit::chain::comment_options_operation, (author)(permlink)(max_accepted_payout)(percent_steem_dollars)(allow_votes)(allow_curation_rewards)(extensions) )

FC_REFLECT( steemit::chain::escrow_transfer_operation, (from)(to)(amount)(memo)(escrow_id)(agent)(fee)(json_meta)(expiration) );
FC_REFLECT( steemit::chain::escrow_dispute_operation, (from)(to)(escrow_id)(who) );
FC_REFLECT( steemit::chain::escrow_release_operation, (from)(to)(escrow_id)(who)(amount) );
FC_REFLECT( steemit::chain::challenge_authority_operation, (challenger)(challenged)(require_owner) );
FC_REFLECT( steemit::chain::prove_authority_operation, (challenged)(require_owner) );
FC_REFLECT( steemit::chain::request_account_recovery_operation, (recovery_account)(account_to_recover)(new_owner_authority)(extensions) );
FC_REFLECT( steemit::chain::recover_account_operation, (account_to_recover)(new_owner_authority)(recent_owner_authority)(extensions) );
FC_REFLECT( steemit::chain::change_recovery_account_operation, (account_to_recover)(new_recovery_account)(extensions) );
