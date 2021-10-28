giraffic
=====================

This is a C++ giraffic test service.

## Releases
* Under `releases` folder

## Features
* async prime number detector
* packet losses detector
* service with http api
* udp server/client
* flatbuffers request/responce serialization 
* logger
* json app config

## Samples 
See `tests` folder

## Usage
http api :

`"GET"    http://server_address/is_prime?number=100200 => HTTP_OK/425`

`"PUT"    http://server_address/add_traffic_point?point=point_address => HTTP_OK/HTTP_BAD_REQUEST`

`"DELETE" http://server_address/rem_traffic_point?point=point_address => HTTP_OK/HTTP_BAD_REQUEST`

`"GET"    http://server_address/traffic_point_lossespoint=point_address =>`

`
{
	"point" : "point_address",
	
	"losses" :
	{
		"average" : "50",
		"current" : "60"
	}
}
`
		
## Protocol
See [schema](src/messages/schema/giraffic.fbs)

## Config
See [config](config/giraffic.json)

`logger_level : info/debug/error`

`logger_sink : console/file`

`http_server : address in form "host:port"`

`transport : udp`

`max_traffic_points : max of traffic loss detection points`

`traffic_packet_delay : traffic packets time period in sec`

`traffic_packets_window : number of traffic packets in time period`

`traffic_packets_slice : current losses evaluation period in sec`

`max_prime_number :  max number for primes detection`

`max_prime_portion : max portion of numbers for async processing`

`max_prime_workers : max workers for async processing`

## Requirements
* A C++ compiler with C++20 support
* POCO
* boost
* Flatbuffers
* gtest

## How to build

At the moment only `cmake-based` build systems are supported.


## How to run
cfg:

create `giraffic.json` in app executable folder

app:

`./giraffic`
