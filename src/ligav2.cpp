#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>

// ===================== ESTRUCTURAS =====================

struct ConfigLiga {
    std::string nombreLiga;
    int victoria, empate, derrota;
    std::vector<std::string> equipos;
};

struct Partido {
    std::string fecha;
    std::string local;
    std::string visitante;
    int gl, gv;
};

struct Equipo {
    std::string nombre;
    int PJ=0, PG=0, PE=0, PP=0;
    int GF=0, GC=0, DG=0, pts=0;
};

// ===================== FUNCIONES AUX =====================

std::vector<std::string> split(std::string linea, char delim) {
    std::vector<std::string> partes;
    std::stringstream ss(linea);
    std::string temp;

    while (getline(ss, temp, delim)) {
        partes.push_back(temp);
    }
    return partes;
}

// ===================== LIMPIAR ENTRADA =====================

void limpiarEntrada() {
    std::cin.clear();
    std::cin.ignore(1000, '\n');
}

// ===================== LEER CONFIG =====================

bool leerConfig(ConfigLiga &config) {

    std::ifstream archivo("data/config.txt");

    if (archivo.is_open()) {

        std::string linea;

        while (getline(archivo, linea)) {

            if (linea.empty() || linea[0] == '#') continue;

            std::vector<std::string> partes = split(linea, '=');
            if (partes.size() != 2) continue;

            if (partes[0] == "liga") config.nombreLiga = partes[1];
            else if (partes[0] == "victoria") config.victoria = std::stoi(partes[1]);
            else if (partes[0] == "empate") config.empate = std::stoi(partes[1]);
            else if (partes[0] == "derrota") config.derrota = std::stoi(partes[1]);
            else if (partes[0] == "equipo") config.equipos.push_back(partes[1]);
        }

        archivo.close();
        return true;

    } else {
        std::cout << "Error: no se pudo abrir config.txt\n";
        return false;
    }
}

// ===================== LEER PARTIDOS =====================

std::vector<Partido> leerPartidos() {

    std::vector<Partido> partidos;
    std::ifstream archivo("data/partidos.txt");

    if (archivo.is_open()) {

        std::string linea;

        while (getline(archivo, linea)) {

            std::vector<std::string> p = split(linea, '|');
            if (p.size() != 5) continue;

            Partido partido;

            partido.fecha = p[0];
            partido.local = p[1];
            partido.visitante = p[2];
            partido.gl = std::stoi(p[3]);
            partido.gv = std::stoi(p[4]);

            partidos.push_back(partido);
        }

        archivo.close();

    } else {
        std::cout << "No se pudo abrir partidos.txt\n";
    }

    return partidos;
}

// ===================== GUARDAR PARTIDO =====================

void guardarPartido(const Partido &p) {

    std::ofstream archivo("data/partidos.txt", std::ios::app);

    if (archivo.is_open()) {

        archivo << p.fecha << "|" << p.local << "|" << p.visitante
                << "|" << p.gl << "|" << p.gv << "\n";

        archivo.close();

    } else {
        std::cout << "Error al guardar partido\n";
    }
}

// ===================== GUARDAR EN FECHAS =====================

void guardarEnFechas(const Partido &p) {

    std::ofstream archivo("data/fechas.txt", std::ios::app);

    if (archivo.is_open()) {

        archivo << "PARTIDO: "
                << p.local << " "
                << p.gl << " - "
                << p.gv << " "
                << p.visitante << "\n";

        archivo.close();

    } else {
        std::cout << "Error al abrir fechas.txt\n";
    }
}

// ===================== MOSTRAR FECHAS =====================

void mostrarFechas() {

    std::ifstream archivo("data/fechas.txt");

    if (archivo.is_open()) {

        std::string linea;

        while (getline(archivo, linea)) {
            std::cout << linea << "\n";
        }

        archivo.close();

    } else {
        std::cout << "No hay historial o no se pudo abrir fechas.txt\n";
    }
}

// ===================== TABLA =====================

void actualizarEquipo(Equipo *e, int gf, int gc, const ConfigLiga &config) {

    e->PJ++;
    e->GF += gf;
    e->GC += gc;
    e->DG = e->GF - e->GC;

    if (gf > gc) {
        e->PG++;
        e->pts += config.victoria;
    }
    else if (gf == gc) {
        e->PE++;
        e->pts += config.empate;
    }
    else {
        e->PP++;
        e->pts += config.derrota;
    }
}

// ===================== CONSTRUIR TABLA =====================

std::vector<Equipo> construirTabla(const std::vector<Partido> &partidos, const ConfigLiga &config) {

    std::vector<Equipo> tabla;

    for (std::string nombre : config.equipos) {
        Equipo e;
        e.nombre = nombre;
        tabla.push_back(e);
    }

    for (const Partido &p : partidos) {
        for (auto &e : tabla) {

            if (e.nombre == p.local)
                actualizarEquipo(&e, p.gl, p.gv, config);

            if (e.nombre == p.visitante)
                actualizarEquipo(&e, p.gv, p.gl, config);
        }
    }

    return tabla;
}

// ===================== ORDENAR TABLA =====================

void ordenarTabla(std::vector<Equipo> &tabla) {

    std::sort(tabla.begin(), tabla.end(), [](const Equipo &a, const Equipo &b) {

        if (a.pts != b.pts) return a.pts > b.pts;
        if (a.DG != b.DG) return a.DG > b.DG;
        return a.GF > b.GF;
    });
}

// ===================== MOSTRAR TABLA =====================

void mostrarTabla(const std::vector<Equipo> &tabla) {

    std::cout << "\n# Equipo        PJ PG PE PP GF GC DG PTS\n";

    for (const auto &e : tabla) {
        std::cout << std::left << std::setw(15) << e.nombre
                  << e.PJ << " "
                  << e.PG << " "
                  << e.PE << " "
                  << e.PP << " "
                  << e.GF << " "
                  << e.GC << " "
                  << e.DG << " "
                  << e.pts << "\n";
    }
}

// ===================== GUARDAR TABLA =====================

void guardarTabla(const std::vector<Equipo> &tabla, const std::string &liga) {

    std::ofstream archivo("data/tabla.txt");

    if (archivo.is_open()) {

        archivo << "# TABLA DE POSICIONES - " << liga << "\n\n";
        archivo << "Pos Equipo               PJ PG PE PP GF GC DG PTS\n";

        int pos = 1;

        for (const auto &e : tabla) {
            archivo << pos++ << "   "
                    << std::setw(20) << std::left << e.nombre
                    << e.PJ << "  "
                    << e.PG << "  "
                    << e.PE << "  "
                    << e.PP << "  "
                    << e.GF << "  "
                    << e.GC << "  "
                    << e.DG << "  "
                    << e.pts << "\n";
        }

        archivo.close();

    } else {
        std::cout << "Error al guardar tabla\n";
    }
}

// ===================== REGISTRAR PARTIDO =====================

void registrarPartido(const ConfigLiga &config) {

    Partido p;

    std::cout << "Fecha: ";
    std::cin >> p.fecha;
    if (std::cin.fail()) { limpiarEntrada(); return; }

    for (int i = 0; i < config.equipos.size(); i++) {
        std::cout << i << ". " << config.equipos[i] << "\n";
    }

    int l, v;

    std::cout << "Local: ";
    std::cin >> l;
    if (std::cin.fail()) { limpiarEntrada(); return; }

    std::cout << "Visitante: ";
    std::cin >> v;
    if (std::cin.fail()) { limpiarEntrada(); return; }

    if (l == v || l < 0 || v < 0 || l >= config.equipos.size() || v >= config.equipos.size()) {
        std::cout << "Seleccion invalida\n";
        return;
    }

    p.local = config.equipos[l];
    p.visitante = config.equipos[v];

    std::cout << "Goles local: ";
    std::cin >> p.gl;
    if (std::cin.fail()) { limpiarEntrada(); return; }

    std::cout << "Goles visitante: ";
    std::cin >> p.gv;
    if (std::cin.fail()) { limpiarEntrada(); return; }

    guardarPartido(p);
    guardarEnFechas(p);

    std::cout << "Partido guardado\n";
}

// ===================== MENU =====================

int menu(const std::string &liga) {

    int op;

    std::cout << "\n==== " << liga << " ====\n";
    std::cout << "1. Ver tabla\n";
    std::cout << "2. Registrar partido\n";
    std::cout << "3. Ver fechas\n";
    std::cout << "4. Salir\n";

    std::cin >> op;

    if (std::cin.fail()) {
        limpiarEntrada();
        return 0;
    }

    return op;
}

// ===================== MAIN =====================

int main() {

    ConfigLiga config;

    if (!leerConfig(config)) return 1;

    while (true) {

        int op = menu(config.nombreLiga);

        if (op == 1) {

            std::vector<Partido> partidos = leerPartidos();
            std::vector<Equipo> tabla = construirTabla(partidos, config);

            ordenarTabla(tabla);
            mostrarTabla(tabla);
            guardarTabla(tabla, config.nombreLiga);
        }

        else if (op == 2) {
            registrarPartido(config);
        }

        else if (op == 3) {
            mostrarFechas();
        }

        else if (op == 4) {
            std::cout << "Saliendo...\n";
            break;
        }

        else {
            std::cout << "Opcion invalida\n";
        }
    }

    return 0;
}
