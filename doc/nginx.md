Configure Nginx
===============

Hermod is not able to handle HTTP request directly, a web server must be
installed as frontend. As Nginx can handle lot of connections with small
amount of memory, it's a good candidate. This document show how to configure
it.

Basic usage
-----------

To serve Hermod contents, you need to define one or more URI for which Nginx
will call the application server using FastCGI protocol. This can be made
using two directives (*rewrite* and *fastcgi_pass*) into a *location* block.

By default, all HTTP headers and content are catched. To ask Nginx to
transmit some additional informations, the *fastcgi_param* directive must
be used. Each of them expose one variable to the FastCGI environment. At
least two variables are required :
* QUERY_STRING used to get URI and arguments
* REQUEST_METHOD used to detect if the request contains datas (GET, POST)
or informations (HEAD, OPTIONS)

Also, two more wariable can optionally be added if the application use
different mime types :
* ACCEPT used by Request class to handle the list of accepted mime
* CONTENT_TYPE used to determine the mime type additional received datas

Here, an example of the minimal nginx server configuration for Hermod :
```
server {
    root /tmp;
    server_name localhost;
    index index.html;
    location / {
        rewrite '^/(.*)$' /hermod?$1 break;

        # By default, Hermod listen on port 9000
        fastcgi_pass   127.0.0.1:9000;

        # Define FCGI environment
        fastcgi_param  GATEWAY_INTERFACE  CGI/1.1;
        fastcgi_param  QUERY_STRING       $query_string;
        fastcgi_param  REQUEST_METHOD     $request_method;
        # Optional (but usefull) parameters
        #fastcgi_param  ACCEPT             $http_accept;
        #fastcgi_param  CONTENT_TYPE       $content_type;
,    }
}
```

Using CORS
----------

When hermod is used as web service with, for example REST api, it should be
necessary to handle CORS headers. Hermod automatically set *allow-origin* and
*allow-method* if headers are present into the request. To expose this
headers, two variables must be exposed by Nginx (Origin and
Access-Control-Request-Method).

Here, an example of the nginx configuration directives :
```
        fastcgi_param  HTTP_ORIGIN        $http_origin;
        fastcgi_param  HTTP_CORS_METHOD   $http_access_control_request_method;
```
