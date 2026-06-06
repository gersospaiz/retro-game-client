#include "ApiClient.h"
#include "json.hpp"
#include <curl/curl.h>
#include <utility>

using json = nlohmann::json;

ApiClient::ApiClient(std::string baseUrl, std::string apiKey, std::string codigoJuego)
    : baseUrl(std::move(baseUrl)),
      apiKey(std::move(apiKey)),
      codigoJuego(std::move(codigoJuego)),
      userToken("") {}

static size_t escribirRespuesta(void* contenido, size_t size, size_t nmemb, std::string* out) {
    size_t total = size * nmemb;
    out->append((char*)contenido, total);
    return total;
}

static bool validarRespuesta(CURLcode res, long httpCode, const std::string& respuesta, json& data, std::string& mensajeError) {
    if (res != CURLE_OK) {
        mensajeError = "Error CURL: " + std::string(curl_easy_strerror(res));
        return false;
    }

    try {
        data = json::parse(respuesta);
    }
    catch (...) {
        mensajeError = "Respuesta no es JSON valido. HTTP " + std::to_string(httpCode) + " | " + respuesta;
        return false;
    }

    if (httpCode < 200 || httpCode >= 300) {
        if (data.contains("message")) {
            mensajeError = data["message"].get<std::string>();
        }
        else {
            mensajeError = "Error HTTP " + std::to_string(httpCode);
        }
        return false;
    }

    if (data.contains("ok") && !data["ok"].get<bool>()) {
        mensajeError = data.value("message", "La API respondio ok=false");
        return false;
    }

    return true;
}

bool ApiClient::probarConexion(std::string& mensajeError) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string respuesta;
    std::string url = baseUrl + "/api/external/info";

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("x-api-key: " + apiKey).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirRespuesta);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    CURLcode res = curl_easy_perform(curl);

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    json data;
    return validarRespuesta(res, httpCode, respuesta, data, mensajeError);
}

bool ApiClient::loginJugador(const std::string& username, const std::string& password, UsuarioApi& usuario, std::string& mensajeError) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string respuesta;
    std::string url = baseUrl + "/api/external/auth/login";

    json body = {
        {"username", username},
        {"password", password}
    };

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("x-api-key: " + apiKey).c_str());

    std::string bodyTexto = body.dump();

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyTexto.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirRespuesta);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    CURLcode res = curl_easy_perform(curl);

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    json data;

    if (!validarRespuesta(res, httpCode, respuesta, data, mensajeError)) {
        return false;
    }

    userToken = data.value("token", "");

    if (userToken.empty()) {
        mensajeError = "La API no devolvio token";
        return false;
    }

    auto usuarioJson = data["usuario"];

    usuario.idUsuario = usuarioJson.value("id_usuario", 0);
    usuario.username = usuarioJson.value("username", "");
    usuario.correo = usuarioJson.value("correo", "");
    usuario.saldoTokens = usuarioJson.value("saldo_tokens", 0);

    return true;
}

bool ApiClient::tieneSesion() const {
    return !userToken.empty();
}

void ApiClient::cerrarSesion() {
    userToken.clear();
}

bool ApiClient::iniciarPartida(PartidaApi& partida, std::string& mensajeError, const std::string& versionJuego, int costoTokens) {
    if (userToken.empty()) {
        mensajeError = "No hay sesion de jugador.";
        return false;
    }

    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string respuesta;
    std::string url = baseUrl + "/api/external/games/start";

    json body = {
        {"codigo_juego", codigoJuego},
        {"version_juego", versionJuego},
        {"costo_tokens", costoTokens}
    };

    std::string auth = "Authorization: Bearer " + userToken;
    std::string bodyTexto = body.dump();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("x-api-key: " + apiKey).c_str());
    headers = curl_slist_append(headers, auth.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyTexto.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirRespuesta);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    CURLcode res = curl_easy_perform(curl);

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    json data;

    if (!validarRespuesta(res, httpCode, respuesta, data, mensajeError)) {
        return false;
    }

    auto p = data["partida"];

    partida.idPartida = p.value("id_partida", -1LL);
    partida.idJuego = p.value("id_juego", 0);
    partida.codigoJuego = data.value("codigo_juego", "");
    partida.nombreJuego = "";
    partida.username = "";
    partida.costoTokens = data.value("costo_tokens", costoTokens);
    partida.saldoAntes = data.value("saldo_antes", 0);
    partida.saldoDespues = data.value("saldo_despues", 0);

    return true;
}

bool ApiClient::reportarScore(long long idPartida, int score, int nivel, std::string& mensajeError) {
    if (userToken.empty()) {
        mensajeError = "No hay sesion de jugador.";
        return false;
    }

    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string respuesta;
    std::string url = baseUrl + "/api/external/games/score";

    json body = {
        {"id_partida", idPartida},
        {"score", score},
        {"nivel", nivel}
    };

    std::string auth = "Authorization: Bearer " + userToken;
    std::string bodyTexto = body.dump();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("x-api-key: " + apiKey).c_str());
    headers = curl_slist_append(headers, auth.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyTexto.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirRespuesta);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    CURLcode res = curl_easy_perform(curl);

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    json data;
    return validarRespuesta(res, httpCode, respuesta, data, mensajeError);
}

bool ApiClient::finalizarPartida(long long idPartida, int scoreFinal, int nivelFinal, const std::string& resultado, int duracionSeg, int tokensGanados, std::string& mensajeError) {
    if (userToken.empty()) {
        mensajeError = "No hay sesion de jugador.";
        return false;
    }

    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string respuesta;
    std::string url = baseUrl + "/api/external/games/finish";

    json body = {
        {"id_partida", idPartida},
        {"score_final", scoreFinal},
        {"nivel_final", nivelFinal},
        {"resultado", resultado},
        {"duracion_seg", duracionSeg},
        {"tokens_ganados", tokensGanados}
    };

    std::string auth = "Authorization: Bearer " + userToken;
    std::string bodyTexto = body.dump();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("x-api-key: " + apiKey).c_str());
    headers = curl_slist_append(headers, auth.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyTexto.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirRespuesta);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    CURLcode res = curl_easy_perform(curl);

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    json data;
    return validarRespuesta(res, httpCode, respuesta, data, mensajeError);
}

bool ApiClient::consultarRanking(std::vector<RankingItem>& ranking, std::string& mensajeError) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string respuesta;
    std::string url = baseUrl + "/api/external/ranking/game/" + codigoJuego;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("x-api-key: " + apiKey).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirRespuesta);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    CURLcode res = curl_easy_perform(curl);

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    json data;

    if (!validarRespuesta(res, httpCode, respuesta, data, mensajeError)) {
        return false;
    }

    ranking.clear();

    for (const auto& item : data["ranking"]) {
        RankingItem r;
        r.username = item.value("username", "");
        r.bestScore = item.value("best_score", 0);
        r.bestNivel = item.value("best_nivel", 0);
        ranking.push_back(r);
    }

    return true;
}