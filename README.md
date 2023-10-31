# Blockchain Project

This project implements a basic blockchain and cryptocurrency in C.

## Features

- Blockchain data structure with linked list
- Block creation with proof of work
- Blockchain serialization and deserialization
- Cryptographic signatures and verification
- Transaction support in blockchain

## Code Structure

The code is structured into the following folders:

- `blockchain` - Implementation of blockchain data structure and related functions
- `crypto` - Cryptographic functions like digital signatures

The main blockchain data structures and functions are:

- `blockchain_.h/c` - Blockchain and block data structure and core functions
- `block_.*c` - Block generation/destruction/validation and related functions

The main crypto functions are:

- `ec_create.c` - Generate ECDSA public/private key pair
- `ec_sign.c` - Sign message with private key
- `ec_verify.c` - Verify signature using public key

The coinbase transaction data structures and functions:

- `struct transaction_s` - Each transaction in a Block is a pointer to this
  structure
- `struct tx_out_s` - Each output in a transaction is a pointer to this
  structure
- `struct tx_in_s` - Each input in a transaction is a pointer to this structure
- `struct unspent_tx_out_s` - Keeps track of unspent transactions in a linked
  list
- `COINBASE_AMOUNT` - Outputs a fixed amount of coins; a macro with 50

- `transaction.h` - Wallet structure and functions

## Usage

The blockchain can be used as:

```c
// Create new blockchain 
blockchain_t *chain = blockchain_create();

// Add new blocks
block_t *block = block_create(/*...*/);
blockchain_add_block(chain, block); 

// Serialize blockchain to file
blockchain_serialize(chain, "chain.dat");

// Deserialize blockchain from file 
blockchain_t *chain2 = blockchain_deserialize("chain.dat");



The crypto signatures are used like:

// Generate key pair
EC_KEY *key = ec_create();

// Sign message
sig_t sig;
ec_sign(key, "hello", 5, &sig); 

// Verify signature
ec_verify(key, "hello", 5, &sig);



Testing
Unit tests for components are in tests/.

Run tests with:

make test



License
This project is licensed under the MIT License - see the LICENSE file for details.

```
