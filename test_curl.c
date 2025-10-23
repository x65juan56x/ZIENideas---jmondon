#include <stdio.h>
#include <curl/curl.h>

int main(void) {
    curl_version_info_data *ver = curl_version_info(CURLVERSION_NOW);
    if (!ver) {
        fprintf(stderr, "No se pudo obtener la versión de libcurl\n");
        return 1;
    }
    printf("libcurl version: %s\n", ver->version);
    printf("SSL backend: %s\n", ver->ssl_version ? ver->ssl_version : "(none)");
    return 0;
}
