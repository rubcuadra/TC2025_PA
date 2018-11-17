<h1>Onitama game</h1>
The project consist of 2 parts a <b>Server</b> and a <b>Client</b><br/>
The server is written in <b>C</b> and is the one in charge of attending incoming connections and playing against them or assigning them a table and be the moderator of the match.<br/>
The client is written in <b>Python</b> and uses Pygame for the GUI, it is a script that connects to the server and sends an option, PVP or PVE, the server will assign a table or play against the player. <br/>

<h3>Environment</h3>

- Server: Tested with <i>clang-1000.11.45.5</i> and <i>gcc 8.2.0</i>, it uses the library of <i>pthread</i> and other standard libraries.<br/>

- Client: Requires <i>Python 3.6.x</i> (String interpolation not allowed by Python <3.6) and <i>Pygame 1.9.4</i>

<h3>Example</h3>

* Server: ./server {PORT_NUMBER}

``` 
    make
    ./server 8989
```

![Init Server](https://github.com/rubcuadra/TC2025_PA/blob/master/Final/screenshots/server.png)

* Client: python client.py {SERVER_ADDRESS} {SERVER_PORT}

```
    python3 client.py 127.0.0.1 8989
```

![Init Client](https://github.com/rubcuadra/TC2025_PA/blob/master/Final/screenshots/client.png)

![Board Example](https://github.com/rubcuadra/TC2025_PA/blob/master/Final/screenshots/GUI.png)

<h3>More</h3>

Read about the project in the [PDF](https://github.com/rubcuadra/TC2025_PA/blob/master/Final/Proposal.pdf)