#pragma once

#include <map>
#include <string>

//
// STATUS CODES
//

// Informational

#define HTTP_CONTINUE 100
#define HTTP_SWITCHING_PROTOCOLS 101
#define HTTP_PROCESSING 102
#define HTTP_EARLY_HINTS 103

// Successful

#define HTTP_OK 200
#define HTTP_CREATED 201
#define HTTP_ACCEPTED 202
#define HTTP_NON_AUTHORITATIVE_INFORMATION 203
#define HTTP_NO_CONTENT 204
#define HTTP_RESET_CONTENT 205
#define HTTP_PARTIAL_CONTENT 206
#define HTTP_MULTI_STATUS 207
#define HTTP_ALREADY_REPORTED 208
#define HTTP_IM_USED 226

// Redirection

#define HTTP_MULTIPLE_CHOICES 300
#define HTTP_MOVED_PERMANENTLY 301
#define HTTP_FOUND 302
#define HTTP_SEE_OTHER 303
#define HTTP_NOT_MODIFIED 304
#define HTTP_USE_PROXY 305
#define HTTP_TEMPORARY_REDIRECT 307
#define HTTP_PERMANENT_REDIRECT 308

// Client error

#define HTTP_BAD_REQUEST 400
#define HTTP_UNAUTHORIZED 401
#define HTTP_PAYMENT_REQUIRED 402
#define HTTP_FORBIDDEN 403
#define HTTP_NOT_FOUND 404
#define HTTP_METHOD_NOT_ALLOWED 405
#define HTTP_NOT_ACCEPTABLE 406
#define HTTP_PROXY_AUTHENTICATION_REQUIRED 407
#define HTTP_REQUEST_TIMEOUT 408
#define HTTP_CONFLICT 409
#define HTTP_GONE 410
#define HTTP_LENGTH_REQUIRED 411
#define HTTP_PRECONDITION_FAILED 412
#define HTTP_CONTENT_TOO_LARGE 413
#define HTTP_URI_TOO_LONG 414
#define HTTP_UNSUPPORTED_MEDIA_TYPE 415
#define HTTP_RANGE_NOT_SATISFIABLE 416
#define HTTP_EXPECTATION_FAILED 417
#define HTTP_MISDIRECTED_REQUEST 421
#define HTTP_UNPROCESSABLE_ENTITY 422
#define HTTP_LOCKED 423
#define HTTP_FAILED_DEPENDENCY 424
#define HTTP_TOO_EARLY 425
#define HTTP_UPGRADE_REQUIRED 426
#define HTTP_PRECONDITIONS_REQUIRED 428
#define HTTP_TOO_MANY_REQUESTS 429
#define HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE 431
#define HTTP_UNAVAILABLE_FOR_LEGAL_REASONS 451

// Server error

#define HTTP_INTERNAL_SERVER_ERROR 500
#define HTTP_NOT_IMPLEMENTED 501
#define HTTP_BAD_GATEWAY 502
#define HTTP_SERVICE_UNAVAILABLE 503
#define HTTP_GATEWAY_TIMEOUT 504
#define HTTP_VERSION_NOT_SUPPORTED 505
#define HTTP_VARIANT_ALSO_NEGOTIATES 506
#define HTTP_INSUFFICIENT_STORAGE 507
#define HTTP_LOOP_DETECTED 508
#define HTTP_NOT_EXTENDED 510
#define HTTP_NETWORK_AUTHENTICATION_REQUIRED 511

// Other

#define HTTP_I_AM_A_TEAPOT 418

//
// MESSAGES
//

// Informational

#define HTTP_CONTINUE_MSG "100 Continue"
#define HTTP_SWITCHING_PROTOCOLS_MSG "101 Switching Protocols"
#define HTTP_PROCESSING_MSG "102 Processing"
#define HTTP_EARLY_HINTS_MSG "103 Early Hints"

// Successful

#define HTTP_OK_MSG "200 OK"
#define HTTP_CREATED_MSG "201 Created"
#define HTTP_ACCEPTED_MSG "202 Accepted"
#define HTTP_NON_AUTHORITATIVE_INFORMATION_MSG "203 Non-authoritative Information"
#define HTTP_NO_CONTENT_MSG "204 No Content"
#define HTTP_RESET_CONTENT_MSG "205 Reset Content"
#define HTTP_PARTIAL_CONTENT_MSG "206 Partial Content"
#define HTTP_MULTI_STATUS_MSG "207 Multi-Status"
#define HTTP_ALREADY_REPORTED_MSG "208 Already Reported"
#define HTTP_IM_USED_MSG "226 IM Used"

// Redirection

#define HTTP_MULTIPLE_CHOICES_MSG "300 Multiple Choices"
#define HTTP_MOVED_PERMANENTLY_MSG "301 Moved Permanently"
#define HTTP_FOUND_MSG "302 Found"
#define HTTP_SEE_OTHER_MSG "303 See Other"
#define HTTP_NOT_MODIFIED_MSG "304 Not Modified"
#define HTTP_USE_PROXY_MSG "305 Use Proxy"
#define HTTP_TEMPORARY_REDIRECT_MSG "307 Temporary Redirect"
#define HTTP_PERMANENT_REDIRECT_MSG "308 Permanent Redirect"

// Client error

#define HTTP_BAD_REQUEST_MSG "400 Bad Request"
#define HTTP_UNAUTHORIZED_MSG "401 Unauthorized"
#define HTTP_PAYMENT_REQUIRED_MSG "402 Payment Required"
#define HTTP_FORBIDDEN_MSG "403 Forbidden"
#define HTTP_NOT_FOUND_MSG "404 Not Found"
#define HTTP_METHOD_NOT_ALLOWED_MSG "405 Method Not Allowed"
#define HTTP_NOT_ACCEPTABLE_MSG "406 Not Acceptable"
#define HTTP_PROXY_AUTHENTICATION_REQUIRED_MSG "407 Proxy Authentication Required"
#define HTTP_REQUEST_TIMEOUT_MSG "408 Request Timeout"
#define HTTP_CONFLICT_MSG "409 Conflict"
#define HTTP_GONE_MSG "410 Gone"
#define HTTP_LENGTH_REQUIRED_MSG "411 Length Required"
#define HTTP_PRECONDITION_FAILED_MSG "412 Precondition Failed"
#define HTTP_CONTENT_TOO_LARGE_MSG "413 Content Too Large"
#define HTTP_URI_TOO_LONG_MSG "414 URI Too Long"
#define HTTP_UNSUPPORTED_MEDIA_TYPE_MSG "415 Unsupported Media Type"
#define HTTP_RANGE_NOT_SATISFIABLE_MSG "416 Range Not Satisfiable"
#define HTTP_EXPECTATION_FAILED_MSG "417 Expectation Failed"
#define HTTP_MISDIRECTED_REQUEST_MSG "421 Misdirected Request"
#define HTTP_UNPROCESSABLE_ENTITY_MSG "422 Unprocessable Content"
#define HTTP_LOCKED_MSG "423 Locked"
#define HTTP_FAILED_DEPENDENCY_MSG "424 Failed Dependency"
#define HTTP_TOO_EARLY_MSG "425 Too Early"
#define HTTP_UPGRADE_REQUIRED_MSG "426 Upgrade Required"
#define HTTP_PRECONDITIONS_REQUIRED_MSG "428 Precondition Required"
#define HTTP_TOO_MANY_REQUESTS_MSG "429 Too Many Requests"
#define HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE_MSG "431 Request Header Fields Too large"
#define HTTP_UNAVAILABLE_FOR_LEGAL_REASONS_MSG "451 Unavailable For Legal Reasons"

// Server error

#define HTTP_INTERNAL_SERVER_ERROR_MSG "500 Internal Server Error"
#define HTTP_NOT_IMPLEMENTED_MSG "501 Not Implemented"
#define HTTP_BAD_GATEWAY_MSG "502 Bad Gateway"
#define HTTP_SERVICE_UNAVAILABLE_MSG "503 Service Unavailable"
#define HTTP_GATEWAY_TIMEOUT_MSG "504 Gateway Timeout"
#define HTTP_VERSION_NOT_SUPPORTED_MSG "505 HTTP Version Not Supported"
#define HTTP_VARIANT_ALSO_NEGOTIATES_MSG "506 Variant Also Negotiates"
#define HTTP_INSUFFICIENT_STORAGE_MSG "507 Insufficient Storage"
#define HTTP_LOOP_DETECTED_MSG "508 Loop Detected"
#define HTTP_NOT_EXTENDED_MSG "510 Not Extended"
#define HTTP_NETWORK_AUTHENTICATION_REQUIRED_MSG "511 Network Authentication Required"

// Other

#define HTTP_I_AM_A_TEAPOT_MSG "418 I'm A Teapot"

namespace http {

std::map<int, std::string> new_m_status_codes();

typedef std::map<int, std::string>::const_iterator m_status_codes_iterator_t;

extern const std::map<int, std::string> g_m_status_codes;

bool is_valid_error_code(int32_t code);

}  // namespace http
