#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <string>
#include <vector>

struct UsuarioApi {
    int idUsuario = 0;
    std::string username;
    std::string correo;
    int saldoTokens = 0;
};

struct PartidaApi {
    long long idPartida = -1;
    int idJuego = 0;
    std::string codigoJuego;
    std::string nombreJuego;
    std::string username;
    int costoTokens = 0;
    int saldoAntes = 0;
    int saldoDespues = 0;
};

struct RankingItem {
    std::string username;
    int bestScore = 0;
    int bestNivel = 0;
};

class ApiClient {
private:
    std::string baseUrl;
    std::string apiKey;
    std::string codigoJuego;
    std::string userToken;

public:
    ApiClient(std::string baseUrl, std::string apiKey, std::string codigoJuego);

    bool probarConexion(std::string& mensajeError);

    bool loginJugador(
        const std::string& username,
        const std::string& password,
        UsuarioApi& usuario,
        std::string& mensajeError
    );

    bool tieneSesion() const;

    void cerrarSesion();

    bool iniciarPartida(
        PartidaApi& partida,
        std::string& mensajeError,
        const std::string& versionJuego = "1.0.0",
        int costoTokens = 5
    );

    bool reportarScore(
        long long idPartida,
        int score,
        int nivel,
        std::string& mensajeError
    );

    bool finalizarPartida(
        long long idPartida,
        int scoreFinal,
        int nivelFinal,
        const std::string& resultado,
        int duracionSeg,
        int tokensGanados,
        std::string& mensajeError
    );

    bool consultarRanking(
        std::vector<RankingItem>& ranking,
        std::string& mensajeError
    );
};

#endif