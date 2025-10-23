#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <curl/curl.h>

static size_t print_response(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	(void)userdata;
	return fwrite(ptr, size, nmemb, stdout);
}

int	prepare_prompt(const char *prompt, char **msg)
{
	size_t		msg_len;
	const char	*prefix =
		"Valida si esta idea sirve para un cuento infantil en español. "
		"Responde SOLO 'OK: <razón>' o 'KO: <razón>'. Idea: ";
	msg_len = strlen(prefix) + strlen(prompt) + 1;
	*msg = malloc(msg_len);
	if (!*msg) return 1;
	strcpy(*msg, prefix);
	strcat(*msg, prompt);
	return 0;
}

/* formatea el JSON muy básico para enviar a la IA: ", \, \n, \r, \t */
static char *json_escape_min(const char *msg)
{
	size_t extra = 0;
	for (const unsigned char *p = (const unsigned char *)msg; *p; ++p) {
		if (*p == '"' || *p == '\\') extra++;
		else if (*p == '\n' || *p == '\r' || *p == '\t') extra++;
	}
	size_t len = strlen(msg);
	char *out = malloc(len + extra + 1);
	if (!out) return NULL;
	char *w = out;
	for (const unsigned char *p = (const unsigned char *)msg; *p; ++p) {
		if (*p == '"') { *w++='\\'; *w++='"'; }
		else if (*p == '\\') { *w++='\\'; *w++='\\'; }
		else if (*p == '\n') { *w++='\\'; *w++='n'; }
		else if (*p == '\r') { *w++='\\'; *w++='r'; }
		else if (*p == '\t') { *w++='\\'; *w++='t'; }
		else { *w++ = *p; }
	}
	*w = '\0';
	return out;
}

/* Lee el prompt desde STDIN (una línea), quita el salto de línea */
static char *read_prompt_stdin(void)
{
	char *line = NULL;
	size_t cap = 0;
	ssize_t nread = getline(&line, &cap, stdin);
	if (nread <= 0) {
		free(line);
		return NULL;
	}
	if (nread > 0 && line[nread - 1] == '\n')
		line[nread - 1] = '\0';
	return line; /* memoria propiedad del llamador */
}

/* Construye el cuerpo JSON mínimo para HF: {"inputs":"..."} */
static char *build_body(const char *escaped_inputs)
{
	const char *fmt = "{\"inputs\":\"%s\"}";
	size_t body_len = strlen(fmt) + strlen(escaped_inputs) + 1;
	char *body = malloc(body_len);
	if (!body) return NULL;
	snprintf(body, body_len, fmt, escaped_inputs);
	return body;
}

/* Construye los headers necesarios */
static struct curl_slist *build_headers(const char *token)
{
	struct curl_slist *headers = NULL;
	char auth_hdr[512];
	snprintf(auth_hdr, sizeof(auth_hdr), "Authorization: Bearer %s", token);
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, auth_hdr);
	return headers;
}

/* Envía la petición HTTP POST y escribe la respuesta a stdout */
static int send_request(const char *url, struct curl_slist *headers, const char *body)
{
	CURL *curl = NULL;
	CURLcode rc;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (!curl) {
		curl_global_cleanup();
		return 1;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, print_response);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

	rc = curl_easy_perform(curl);
	if (rc != CURLE_OK)
		fprintf(stderr, "\nError cURL: %s\n", curl_easy_strerror(rc));
	else
		fprintf(stdout, "\n");

	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return (rc == CURLE_OK) ? 0 : 2;
}


int	main(void)
{
	/* 1) Leer prompt desde STDIN */
	printf("Introduce la idea para el cuento y pulsa Enter:\n");
	char *prompt = read_prompt_stdin();
	if (!prompt || !*prompt) {
		fprintf(stderr, "Error: entrada vacía\n");
		free(prompt);
		return 1;
	}

	/* 2) Token de HF */
	const char *token = getenv("HUGGINGFACE_TOKEN");
	if (!token || !*token) {
		fprintf(stderr, "Falta HUGGINGFACE_TOKEN (export HUGGINGFACE_TOKEN=hf_xxx)\n");
		free(prompt);
		return 1;
	}

	/* 3) Preparar mensaje para la IA */
	char *msg = NULL;
	if (prepare_prompt(prompt, &msg) != 0) {
		fprintf(stderr, "Error preparando el mensaje\n");
		free(prompt);
		return 1;
	}
	free(prompt);

	/* 4) Escapar JSON y construir cuerpo */
	char *esc = json_escape_min(msg);
	free(msg);
	if (!esc) {
		fprintf(stderr, "Error escapando JSON\n");
		return 1;
	}
	char *body = build_body(esc);
	free(esc);
	if (!body) {
		fprintf(stderr, "Error construyendo cuerpo JSON\n");
		return 1;
	}

	/* 5) Headers y envío */
	struct curl_slist *headers = build_headers(token);
	if (!headers) {
		fprintf(stderr, "Error construyendo headers\n");
		free(body);
		return 1;
	}
	const char *url = "https://api-inference.huggingface.co/models/google/gemma-2-2b-it";
	int rc = send_request(url, headers, body);

	curl_slist_free_all(headers);
	free(body);
	return rc;
}