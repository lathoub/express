#pragma once

#define HTTP_STATUS_CONTINUE 100
#define HTTP_STATUS_SWITCH_PROTOCOLS 101
#define HTTP_STATUS_PROCESSING 102
#define HTTP_STATUS_EARLYHINTS 103

#define HTTP_STATUS_OK 200
#define HTTP_STATUS_CREATED 201
#define HTTP_STATUS_ACCEPTED 202
#define HTTP_STATUS_PARTIAL 203
#define HTTP_STATUS_NO_CONTENT 204
#define HTTP_STATUS_RESET_CONTENT 205
#define HTTP_STATUS_PARTIAL_CONTENT 206
#define HTTP_STATUS_MULTI_STATUS 207
#define HTTP_STATUS_ALREADY_REPORTED 208

#define HTTP_STATUS_AMBIGUOUS 300
#define HTTP_STATUS_MOVED 301
#define HTTP_STATUS_REDIRECT 302
#define HTTP_STATUS_REDIRECT_METHOD 303
#define HTTP_STATUS_NOT_MODIFIED 304
#define HTTP_STATUS_USE_PROXY 305
#define HTTP_STATUS_REDIRECT_KEEP_VERB 307

#define HTTP_STATUS_BAD_REQUEST 400
#define HTTP_STATUS_DENIED 401
#define HTTP_STATUS_PAYMENT_REQ 402
#define HTTP_STATUS_FORBIDDEN 403
#define HTTP_STATUS_NOT_FOUND 404
#define HTTP_STATUS_BAD_METHOD 405
#define HTTP_STATUS_NONE_ACCEPTABLE 406
#define HTTP_STATUS_PROXY_AUTH_REQ 407
#define HTTP_STATUS_REQUEST_TIMEOUT 408
#define HTTP_STATUS_CONFLICT 409
#define HTTP_STATUS_GONE 410
#define HTTP_STATUS_LENGTH_REQUIRED 411
#define HTTP_STATUS_PRECOND_FAILED 412
#define HTTP_STATUS_REQUEST_TOO_LARGE 413
#define HTTP_STATUS_URI_TOO_LONG 414
#define HTTP_STATUS_UNSUPPORTED_MEDIA 415
#define HTTP_STATUS_RETRY_WITH 449

#define HTTP_STATUS_SERVER_ERROR 500
#define HTTP_STATUS_NOT_SUPPORTED 501
#define HTTP_STATUS_BAD_GATEWAY 502
#define HTTP_STATUS_SERVICE_UNAVAIL 503
#define HTTP_STATUS_GATEWAY_TIMEOUT 504
#define HTTP_STATUS_VERSION_NOT_SUP 505
#define HTTP_STATUS_VARIANT_ALSO_NEGOTIATES 506
#define HTTP_STATUS_INSUFFICIANT_STORAGE 507
#define HTTP_STATUS_LOOP_DETECTED 508
#define HTTP_STATUS_NOT_EXTENDED 510
