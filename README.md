# SIT (Streaming Index Toy)

SIT is a lightweight TCP server that provides real-time full-text search over
streams of JSON documents. It's also usable as a C library, where it can parse
any stream using custom parsers.

## Why?

* **Lightweight**
  The executable is 228K on my machine.  It only depends on libev and libc;
  and currently (and temporarily) uses Ruby for the build and test suites.
* **True realtime**
  This is "true" real time, as opposed to "near" real time. SIT eliminates
  garbage collection, "commits," "flushes," and fsyncs. This is search highly
  optimized for recency and real-time. (However, we have not gone as far as 
  hardware realtime or kernel guarantees)
* **Write, then read**
  Trivially support search results which include the document you _just_ added.
* **Computationally efficient percolation**
  SIT is designed to add documents and execute query callbacks efficiently with
  up to 100,000 registered queries and/or connections.
* **Flexible pubsub**
  Rather than publishing into channels, you can use SIT to publish into a global
  event stream, and subscribe to dynamic channels defined by search queries.

## Protocol

SIT speaks a simple line-based pipelineable protocol.  Any line that starts with
a curly brace (`{`) is interpreted as a JSON document to add to the indexed
dataset. Other lines are interpreted as commands.

All responses are JSON lines, and have a `status` key, which can be either ok,
or error.  Other keys are implemented on a per-command basis.

## Search & Percolation

SIT provides traditional search, where you give a query and get a resultset
back.  It also provides percolation, where you can register a query that will
notify you when matching documents are added to the index.

## Commands

* **register** _QUERY_    
  Registers the query, or queries, for percolation.  When any document that
  matches the query is added, SIT will print a "found" response in this stream.

  Sample request/response:    
  
        > register title ~ "hello world" AND points > 4;
        < {"status": "ok", "message": "registered", "id": 29}
        # ...
        < {"status": "ok", "message": "found", "query_id": 29, "doc_id": 500, "doc": {"title": "hello sweet world", "points": 7}}
  
  The response to a `register` command includes an ID of the registered query.

* **unregister** _QUERYID_    
  Give the id provided in the **register** response, to stop the percolation.

  Sample request/response:    
  
        > unregister 29
        < {"status": "ok", "message": "unregistered", "id": 29}

* **query** _QUERY_    
  Do a search.
  
  Sample request/response:

        # adding some docs
        > {"hello":"world 0"}
        > {"hello":"world 1"}
        > {"hello":"world 2"}
        > {"hello":"world 3"}
        > {"hello":"world 4"}
        > {"hello":"world 5"}
        > {"hello":"world 6"}
        > {"hello":"world 7"}
        > {"hello":"world 8"}
        > {"hello":"world 9"}
        >
        > query hello ~ world LIMIT 5;
        < {"status": "ok", "message": "querying", "id": 27}
        < {"status": "ok", "message": "found", "query_id": 27, "doc_id": 9, "doc": {"hello":"world 9"}}
        < {"status": "ok", "message": "found", "query_id": 27, "doc_id": 8, "doc": {"hello":"world 8"}}
        < {"status": "ok", "message": "found", "query_id": 27, "doc_id": 7, "doc": {"hello":"world 7"}}
        < {"status": "ok", "message": "found", "query_id": 27, "doc_id": 6, "doc": {"hello":"world 6"}}
        < {"status": "ok", "message": "found", "query_id": 27, "doc_id": 5, "doc": {"hello":"world 5"}}
        < {"status": "ok", "message": "complete", "id": 27}

## Query Language

SIT has a simple query language, composed of boolean operations (`AND`, `OR`,
`NOT`) over clauses. You can append `LIMIT N` to the end of a query. Queries
are terminated with either a newline or a semicolon. The following are valid
clauses:

      field_name ~ string 
      field_name > integer
      field_name = integer
      field_name < integer
      field_name >= integer
      field_name <= integer
      field_name != integer

## Tokenization

SIT is designed to use pluggable tokenization strategies.

__TODO: Describe tokenization 101 basics, concepts, examples.__

#### Supported tokenization strategies

- [x] whitespace
- [ ] Unicode UAX#29 word boundaries
- [ ] regular expression pattern tokenizer

Pull requests are welcome.

## What is the tilde?

The tilde indicates a full-text search. A full-text search identifies documents
where a given term is present in the specified field.

The full-text search, `title ~ hello` will match JSON documents with a field
named `title` which contain a token of `hello`. For example, the document
`{"title":"hello full text search"}` when tokenized with a simple whitespace
tokenizer.

A quoted value for the tilde operator will match documents where all the terms
of the quoted text are present in the named field. For example, `title ~ "hello
world"` will be transformed into `(title ~ hello AND title ~ world)`.

## TODO

* [x] Nested JSON support
* Composable streams
* Persistance
* Replace
* wchar/unicode support
* Facets
* Aggregations
* Quoted/span/proximity queries
* selectable parsers besides JSON in the server? (e.g., log output, msgpack,
  BSON)

## Quick Start

Feeling adventurous? Run SIT on your own system and try some of these demos.

### Downloading and building SIT

```
git clone git://github.com/fizx/sit.git
cd sit
bundle install
bundle exec rake
```

### Demo: Running SIT with simple inputs and searches

Start the server with `./sit`, which reads commands from standard input, and
prints its output to standard out.

```sh
./sit
{"hello":"world"}
# {"status": "ok", "message": "added", "doc_id": 0"}
query hello ~ world;
# {"status": "ok", "message": "querying", "id": 0}
# {"status": "ok", "message": "found", "query_id": 0, "doc_id": 0, "doc":
# {"hello":"world"}}
# {"status": "ok", "message": "complete", "id": 0}
```


### Demo: Twitter Streaming API

Reminder, this is pre-release software. We use Twitter Streaming APIs as testing
to find new and novel edge cases which cause crashes.

You should assume that this demo has a 50/50 chance to format your hard drive,
and proceed accordingly. Preferably by reading the source to see whether this
paragraph is serious or not.

First, [install and authenticate twurl](https://github.com/marcel/twurl).

Next, start a server listening to the network.

```
./sit --port 4000
[INFO] [2013:03:0221:15:53] Successfully started server.
```

Now stream documents from Twitter, via netcat, to your running server.

```
twurl -t -H stream.twitter.com /1/statuses/sample.json | nc localhost 4000
```

