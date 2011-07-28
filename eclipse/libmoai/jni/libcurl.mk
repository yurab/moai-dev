#================================================================#
# libcurl
#================================================================#
common_CFLAGS := -DHAVE_CONFIG_H -DUSE_SSLEAY -DUSE_OPENSSL
include $(CLEAR_VARS) 
include $(SRC_PATH)/3rdparty/curl-7.21.7/src/Makefile.inc 
MY_SRCS := file.c timeval.c base64.c hostip.c progress.c formdata.c	\
  cookie.c http.c sendf.c ftp.c url.c dict.c if2ip.c speedcheck.c	\
  ldap.c ssluse.c version.c getenv.c escape.c mprintf.c telnet.c	\
  netrc.c getinfo.c transfer.c strequal.c easy.c security.c krb4.c	\
  curl_fnmatch.c fileinfo.c ftplistparser.c wildcard.c krb5.c		\
  memdebug.c http_chunks.c strtok.c connect.c llist.c hash.c multi.c	\
  content_encoding.c share.c http_digest.c md4.c md5.c curl_rand.c	\
  http_negotiate.c http_ntlm.c inet_pton.c strtoofft.c strerror.c	\
  hostasyn.c hostip4.c hostip6.c hostsyn.c inet_ntop.c parsedate.c	\
  select.c gtls.c sslgen.c tftp.c splay.c strdup.c socks.c ssh.c nss.c	\
  qssl.c rawstr.c curl_addrinfo.c socks_gssapi.c socks_sspi.c		\
  curl_sspi.c slist.c nonblock.c curl_memrchr.c imap.c pop3.c smtp.c	\
  pingpong.c rtsp.c curl_threads.c warnless.c hmac.c polarssl.c		\
  curl_rtmp.c openldap.c curl_gethostname.c gopher.c axtls.c		\
  idn_win32.c http_negotiate_sspi.c cyassl.c http_proxy.c non-ascii.c	\
  asyn-ares.c asyn-thread.c

MY_CURL_HEADERS := \
    curlbuild.h \
    curl.h \
    curlrules.h \
    curlver.h \
    easy.h \
    mprintf.h \
    multi.h \
	setup.h \
    stdcheaders.h \
    typecheck-gcc.h \
    types.h 

LOCAL_CFLAGS += $(common_CFLAGS) 
LOCAL_COPY_HEADERS_TO := libcurl 
LOCAL_COPY_HEADERS := $(addprefix $(SRC_PATH)/3rdparty/curl-7.21.7/include-android/curl/,$(MY_CURL_HEADERS)) 
LOCAL_MODULE:= libcurl 

# header search paths
LOCAL_C_INCLUDES += $(SRC_PATH)/3rdparty/curl-7.21.7/include-android
LOCAL_C_INCLUDES += $(SRC_PATH)/3rdparty/curl-7.21.7/include-android/curl
LOCAL_C_INCLUDES += $(SRC_PATH)/3rdparty/curl-7.21.7/lib
LOCAL_C_INCLUDES += $(SRC_PATH)/3rdparty/openssl-1.0.0d/include-android

# included files
LOCAL_SRC_FILES := $(addprefix $(SRC_PATH)/3rdparty/curl-7.21.7/lib/,$(MY_SRCS))

include $(BUILD_STATIC_LIBRARY) 
