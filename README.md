SIT -- Stream Indexing Toolkit
==============================

SIT is a lightweight tcp server that provides realtime full text search over streams of (currently flattened only) json documents.  It's also accessible as a c library, where it can be used to parse any stream using custom parsers.

Why?
--------

* **Lightweight**    
  The executable is 228K on my machine.  It only depends on libev and libc (I'm temporarily using Ruby for the build and test suite).
* **True realtime**    
  No garbage collection.  No "commits," "flushes," or fsync requirements.  It's optimized for getting recent documents.
* **Write then read**    
  Realtime means that you can trivially support search results that include the document you just added.
* **Efficient percolation**    
  SIT is designed to add documents and execute query callbacks efficiently with up to 100k registered queries and/or connections.
* **Flexible pubsub**    
  Rather than publishing into channels, you can use SIT to publish into a global event stream, and subscribe to dynamic channels defined by search queries.
  
Protocol
--------

SIT speaks a simple line-based pipelineable protocol.  Any line that starts with { is interpreted as a json document to add to the indexed dataset.  Other lines are interpreted as commands.  
  
All responses are json lines, and have a `status` key, which can be either ok, or error.  Other keys are implemented on a per-command basis.
  
Search & Percolation
--------------------

SIT provides traditional search, where you give a query and get a resultset back.  It also provides percolation, where you can register a query that will notify you when matching documents are added to the index.
  
Commands
--------

* **register** _QUERY_    
  Registers this query(s) for percolation.  When any document that matches the query is added, SIT will print a "found" response in this stream.
  
  Sample request/response:    
  
        > register title ~ "hello world" AND points > 4;
        < {"status": "ok", "message": "registered", "id": 29}
        # ...
        < {"status": "ok", "message": "found", "query_id": 29, "doc_id": 500, "doc": {"title": "hello sweet world", "points": 7}}

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

Query Language
--------------

SIT has a simple query language, composed of boolean operations (AND OR NOT) over clauses.  You can tack on LIMIT N to the end of a query.  Queries are terminated with either a newline or a semicolon.  The following are valid clauses:

      field_name ~ string 
      field_name > integer
      field_name = integer
      field_name < integer
      field_name >= integer
      field_name <= integer
      field_name != integer
      
Tokenization
------------

In this pre-release version, **all strings in the server are tokenized by whitespace only**.  You can customize this if you're using the c-library, and other tokenizer implementations are welcome pull requests.

What is the tilde?
------------------

Tilde means full-text search.  In the simplest case, `title ~ hello` means to match the document if the document has a key called title, it points to the string, and string.split(/\s+/).include?("hello").  `title ~ "hello world"` will be transformed into `(title ~ hello AND title ~ world)`

TODO
----

* Persistance
* Nested json support
* replace 
* wchar/unicode support
* facets
* aggregations
* quoted/span/proximity queries