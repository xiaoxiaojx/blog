gRPC examples
===========================
INSTALL
-------

   ```sh
   $ # Get the gRPC repository
   $ git clone https://github.com/grpc/grpc-node.git
   $ cd examples
   $ npm install
   ```

SIMPLE examples
-------

There are two ways to generate the code needed to work with protocol buffers in Node.js - one approach uses [Protobuf.js](https://github.com/dcodeIO/ProtoBuf.js/) to dynamically generate the code at runtime, the other uses code statically generated using the protocol buffer compiler `protoc`. The examples behave identically, and either server can be used with either client.

 - Run the server

   ```sh
   $ # from this directory
   $ npm run helloworld:server
   $ # OR
   $ npm run helloworld_static:server
   ```

 - Run the client

   ```sh
   $ # from this directory
   $ npm run helloworld:client
   $ # OR
   $ npm run helloworld_static:client
   ```

MORE examples
----
- helloworld_client_stream: A client-side streaming RPC where the client writes a sequence of messages and sends them to the server, again using a provided stream. Once the client has finished writing the messages, it waits for the server to read them all and return its response. You specify a client-side streaming method by placing the stream keyword before the request type.
- helloworld_server_stream: A server-side streaming RPC where the client sends a request to the server and gets a stream to read a sequence of messages back. The client reads from the returned stream until there are no more messages. As you can see in our example, you specify a server-side streaming method by placing the stream keyword before the response type.
- helloworld_stream: A bidirectional streaming RPC where both sides send a sequence of messages using a read-write stream. The two streams operate independently, so clients and servers can read and write in whatever order they like: for example, the server could wait to receive all the client messages before writing its responses, or it could alternately read a message then write a message, or some other combination of reads and writes. The order of messages in each stream is preserved. You specify this type of method by placing the stream keyword before both the request and the response.
- route_guide: A complete example



TUTORIAL
--------
You can find a more detailed tutorial in [gRPC Basics: Node.js][]

[Install gRPC Node]:../../src/node
[gRPC Basics: Node.js]:https://grpc.io/docs/languages/node/basics
