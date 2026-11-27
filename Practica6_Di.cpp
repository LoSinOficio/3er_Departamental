#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <memory>
#include <cstring>
#include <limits>

using namespace std;

// ==========================================
// 1. ARCO DIRIGIDO (SRP)
// ni: origen, nf: destino, a: etiqueta[3], tiempo: int, cost: double
// ==========================================
class ArcoDirigido {
private:
    string ni;
    string nf;
    char a[4];
    int tiempo;
    double cost;

public:
    ArcoDirigido(const string& origen = "",
                 const string& destino = "",
                 const string& etiqueta = "a",
                 int t = 0,
                 double costo = 0.0)
        : ni(origen), nf(destino), tiempo(t), cost(costo) {
        setEtiqueta(etiqueta);
    }

    void setEtiqueta(const string& etq) {
        strncpy(a, etq.c_str(), 3);
        a[3] = '\0';
    }

    string getNi() const { return ni; }
    string getNf() const { return nf; }
    string getA() const { return string(a); }
    int getTiempo() const { return tiempo; }
    double getCost() const { return cost; }

    void setNi(const string& val) { ni = val; }
    void setNf(const string& val) { nf = val; }
    void setTiempo(int val) { tiempo = val; }
    void setCost(double val) { cost = val; }

    string aJSON(int indent = 12) const {
        string pad(indent, ' ');
        return pad + "{\"ni\": \"" + ni +
               "\", \"nf\": \"" + nf +
               "\", \"a\": \"" + getA() +
               "\", \"tiempo\": " + to_string(tiempo) +
               ", \"cost\": " + to_string(cost) + "}";
    }

    void mostrar() const {
        cout << "(" << ni << " -> " << nf
             << ", a:" << getA()
             << ", tiempo:" << tiempo
             << ", cost:" << cost << ")";
    }
};

// ==========================================
// 2. DIGRAFO D = (V, A) Y RUTA PATH (SRP)
// ==========================================
class DigrafoCuaderno {
private:
    string nombreDigrafo;
    vector<string> verticesV;
    vector<ArcoDirigido> arcosA;
    list<ArcoDirigido> path;

public:
    DigrafoCuaderno(const string& nombre = "D1") : nombreDigrafo(nombre) {}

    string getNombre() const { return nombreDigrafo; }
    void setNombre(const string& nombre) { nombreDigrafo = nombre; }

    bool existeVertice(const string& v) const {
        for (const auto& item : verticesV) {
            if (item == v) return true;
        }
        return false;
    }

    void agregarVertice(const string& v) {
        if (!v.empty() && !existeVertice(v)) {
            verticesV.push_back(v);
        }
    }

    bool eliminarVertice(const string& v) {
        for (auto it = verticesV.begin(); it != verticesV.end(); ++it) {
            if (*it == v) {
                verticesV.erase(it);
                return true;
            }
        }
        return false;
    }

    void agregarArcoDirigido(const string& ni, const string& nf, const string& a, int tiempo, double cost) {
        agregarVertice(ni);
        agregarVertice(nf);
        arcosA.emplace_back(ni, nf, a, tiempo, cost);
    }

    bool editarArco(size_t indice, const string& ni, const string& nf, const string& a, int tiempo, double cost) {
        if (indice >= arcosA.size()) return false;
        agregarVertice(ni);
        agregarVertice(nf);
        arcosA[indice].setNi(ni);
        arcosA[indice].setNf(nf);
        arcosA[indice].setEtiqueta(a);
        arcosA[indice].setTiempo(tiempo);
        arcosA[indice].setCost(cost);
        return true;
    }

    bool eliminarArco(size_t indice) {
        if (indice >= arcosA.size()) return false;
        arcosA.erase(arcosA.begin() + static_cast<long>(indice));
        return true;
    }

    void agregarAPath(const string& ni, const string& nf, const string& a, int tiempo, double cost) {
        agregarVertice(ni);
        agregarVertice(nf);
        path.emplace_back(ni, nf, a, tiempo, cost);
    }

    const vector<string>& getVerticesV() const { return verticesV; }
    const vector<ArcoDirigido>& getArcosA() const { return arcosA; }
    const list<ArcoDirigido>& getPath() const { return path; }

    void cargarDatos(const vector<string>& v, const vector<ArcoDirigido>& a, const list<ArcoDirigido>& p) {
        verticesV = v;
        arcosA = a;
        path = p;
    }

    void listarArcos() const {
        if (arcosA.empty()) {
            cout << "\n[AVISO] No hay arcos dirigidos registrados en " << nombreDigrafo << ".\n";
            return;
        }
        cout << "\n--- ARCOS DIRIGIDOS EN [" << nombreDigrafo << "] ---\n";
        for (size_t i = 0; i < arcosA.size(); ++i) {
            cout << i + 1 << ". ";
            arcosA[i].mostrar();
            cout << "\n";
        }
    }

    void mostrarEstructura() const {
        cout << "\n=======================================================================\n";
        cout << "              DIGRAFO [" << nombreDigrafo << "] : D = (V, A) Y PATH            \n";
        cout << "=======================================================================\n";

        cout << "V = { ";
        for (size_t i = 0; i < verticesV.size(); ++i) {
            cout << verticesV[i];
            if (i + 1 < verticesV.size()) cout << ", ";
        }
        cout << " }\n";

        cout << "A = { ";
        for (size_t i = 0; i < arcosA.size(); ++i) {
            cout << arcosA[i].getA();
            if (i + 1 < arcosA.size()) cout << ", ";
        }
        cout << " }\n\n";

        cout << "Arcos dirigidos (ni -> nf):\n";
        if (arcosA.empty()) {
            cout << "  Sin arcos dirigidos registrados.\n";
        } else {
            for (const auto& arco : arcosA) {
                cout << "  ";
                arco.mostrar();
                cout << "\n";
            }
        }

        cout << "\nPath (list<Arco>):\n";
        if (path.empty()) {
            cout << "  Ruta vacia.\n";
        } else {
            int tiempoTotal = 0;
            double costoTotal = 0.0;
            size_t idx = 0;
            for (const auto& paso : path) {
                cout << "  ";
                paso.mostrar();
                if (idx + 1 < path.size()) cout << " ->";
                cout << "\n";
                tiempoTotal += paso.getTiempo();
                costoTotal += paso.getCost();
                ++idx;
            }
            cout << "  Resumen Path -> Tiempo total: " << tiempoTotal
                 << " min | Costo total: $" << costoTotal << "\n";
        }
        cout << "=======================================================================\n";
    }
};

// ==========================================
// 3. INTERFAZ DE PERSISTENCIA - OCP / DIP
// ==========================================
class IPersistenciaColeccionDigrafos {
public:
    virtual void guardarTodos(const string& ruta, const map<string, DigrafoCuaderno>& digrafos) = 0;
    virtual map<string, DigrafoCuaderno> cargarTodos(const string& ruta) = 0;
    virtual ~IPersistenciaColeccionDigrafos() = default;
};

// ==========================================
// 4. IMPLEMENTACION JSON - LSP / SRP
// ==========================================
class ManejadorDigrafoJSON : public IPersistenciaColeccionDigrafos {
private:
    static string limpiar(const string& s) {
        size_t ini = s.find_first_not_of(" \t\r\n");
        if (ini == string::npos) return "";
        size_t fin = s.find_last_not_of(" \t\r\n");
        return s.substr(ini, fin - ini + 1);
    }

    static bool parsearEnteroSeguro(const string& texto, int& salida) {
        string limp = limpiar(texto);
        if (limp.empty()) return false;
        try {
            size_t idx = 0;
            int val = stoi(limp, &idx);
            if (idx != limp.size()) return false;
            salida = val;
            return true;
        } catch (...) {
            return false;
        }
    }

    static bool parsearDoubleSeguro(const string& texto, double& salida) {
        string limp = limpiar(texto);
        if (limp.empty()) return false;
        try {
            size_t idx = 0;
            double val = stod(limp, &idx);
            if (idx != limp.size()) return false;
            salida = val;
            return true;
        } catch (...) {
            return false;
        }
    }

    static string extraerValorCampoString(const string& bloque, const string& clave) {
        string patron = "\"" + clave + "\"";
        size_t pos = bloque.find(patron);
        if (pos == string::npos) return "";
        size_t dosPuntos = bloque.find(':', pos + patron.size());
        if (dosPuntos == string::npos) return "";
        size_t q1 = bloque.find('"', dosPuntos);
        if (q1 == string::npos) return "";
        size_t q2 = bloque.find('"', q1 + 1);
        if (q2 == string::npos) return "";
        return bloque.substr(q1 + 1, q2 - q1 - 1);
    }

    static string extraerValorCampoNumero(const string& bloque, const string& clave) {
        string patron = "\"" + clave + "\"";
        size_t pos = bloque.find(patron);
        if (pos == string::npos) return "";
        size_t dosPuntos = bloque.find(':', pos + patron.size());
        if (dosPuntos == string::npos) return "";
        size_t ini = bloque.find_first_not_of(" \t", dosPuntos + 1);
        if (ini == string::npos) return "";
        size_t fin = bloque.find_first_of(",}", ini);
        string val = (fin == string::npos) ? bloque.substr(ini) : bloque.substr(ini, fin - ini);
        return limpiar(val);
    }

    static ArcoDirigido parsearObjetoArco(const string& obj) {
        ArcoDirigido arco;
        arco.setNi(extraerValorCampoString(obj, "ni"));
        arco.setNf(extraerValorCampoString(obj, "nf"));
        string a = extraerValorCampoString(obj, "a");
        string tiempoStr = extraerValorCampoNumero(obj, "tiempo");
        string costStr = extraerValorCampoNumero(obj, "cost");

        int tiempoVal = 0;
        double costVal = 0.0;
        if (!a.empty()) arco.setEtiqueta(a);
        if (parsearEnteroSeguro(tiempoStr, tiempoVal)) arco.setTiempo(tiempoVal);
        if (parsearDoubleSeguro(costStr, costVal)) arco.setCost(costVal);
        return arco;
    }

public:
    void guardarTodos(const string& ruta, const map<string, DigrafoCuaderno>& digrafos) override {
        ofstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "\n[ERROR] No se pudo abrir el archivo JSON para escritura.\n";
            return;
        }

        archivo << "{\n";
        archivo << "  \"digrafos\": [\n";

        size_t dIdx = 0;
        for (const auto& par : digrafos) {
            const DigrafoCuaderno& digrafo = par.second;
            const auto& v = digrafo.getVerticesV();
            const auto& a = digrafo.getArcosA();
            const auto& p = digrafo.getPath();

            archivo << "    {\n";
            archivo << "      \"nombre\": \"" << digrafo.getNombre() << "\",\n";
            archivo << "      \"D\": {\n";

            archivo << "        \"V\": [";
            for (size_t i = 0; i < v.size(); ++i) {
                archivo << "\"" << v[i] << "\"";
                if (i + 1 < v.size()) archivo << ", ";
            }
            archivo << "],\n";

            archivo << "        \"A\": [";
            for (size_t i = 0; i < a.size(); ++i) {
                archivo << "\"" << a[i].getA() << "\"";
                if (i + 1 < a.size()) archivo << ", ";
            }
            archivo << "],\n";

            archivo << "        \"arcos\": [\n";
            for (size_t i = 0; i < a.size(); ++i) {
                archivo << a[i].aJSON(12);
                if (i + 1 < a.size()) archivo << ",";
                archivo << "\n";
            }
            archivo << "        ],\n";

            archivo << "        \"path\": [\n";
            size_t pIdx = 0;
            for (const auto& paso : p) {
                archivo << paso.aJSON(12);
                if (pIdx + 1 < p.size()) archivo << ",";
                archivo << "\n";
                ++pIdx;
            }
            archivo << "        ]\n";

            archivo << "      }\n";
            archivo << "    }";
            if (dIdx + 1 < digrafos.size()) archivo << ",";
            archivo << "\n";
            ++dIdx;
        }

        archivo << "  ]\n";
        archivo << "}\n";

        archivo.close();
        cout << "\n[EXITO] Todos los digrafos fueron guardados en: " << ruta << "\n";
    }

    map<string, DigrafoCuaderno> cargarTodos(const string& ruta) override {
        map<string, DigrafoCuaderno> resultado;
        ifstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "\n[AVISO] No se encontro el archivo JSON: " << ruta << "\n";
            return resultado;
        }

        string linea;
        string nombreActual = "";
        vector<string> vActual;
        vector<ArcoDirigido> aActual;
        list<ArcoDirigido> pathActual;
        string seccion = "";
        bool enDigrafo = false;

        while (getline(archivo, linea)) {
            string limp = limpiar(linea);
            if (limp.empty()) continue;

            if (limp.find("\"nombre\"") != string::npos) {
                nombreActual = extraerValorCampoString(limp, "nombre");
                vActual.clear();
                aActual.clear();
                pathActual.clear();
                seccion = "";
                enDigrafo = true;
                continue;
            }

            if (!enDigrafo) continue;

            if (limp.find("\"V\"") != string::npos) {
                size_t a1 = limp.find('[');
                size_t a2 = limp.find(']');
                if (a1 != string::npos && a2 != string::npos && a2 > a1) {
                    string contenido = limp.substr(a1 + 1, a2 - a1 - 1);
                    size_t pos = 0;
                    while ((pos = contenido.find('"', pos)) != string::npos) {
                        size_t fin = contenido.find('"', pos + 1);
                        if (fin == string::npos) break;
                        string vert = contenido.substr(pos + 1, fin - pos - 1);
                        if (!vert.empty()) vActual.push_back(vert);
                        pos = fin + 1;
                    }
                }
                continue;
            }

            if (limp.find("\"arcos\"") != string::npos) {
                seccion = "arcos";
                continue;
            }
            if (limp.find("\"path\"") != string::npos) {
                seccion = "path";
                continue;
            }
            if (!seccion.empty() && limp.find("]") != string::npos && limp.find("{") == string::npos) {
                seccion = "";
                continue;
            }

            if (!seccion.empty() && limp.find("{") != string::npos) {
                ArcoDirigido arco = parsearObjetoArco(limp);
                if (seccion == "arcos") aActual.push_back(arco);
                else if (seccion == "path") pathActual.push_back(arco);
                continue;
            }

            if (seccion.empty() && (limp == "}," || (limp == "}" && !nombreActual.empty()))) {
                if (!nombreActual.empty() && resultado.find(nombreActual) == resultado.end()) {
                    DigrafoCuaderno d(nombreActual);
                    d.cargarDatos(vActual, aActual, pathActual);
                    resultado[nombreActual] = d;
                }
            }
        }

        archivo.close();
        cout << "\n[EXITO] Se cargaron " << resultado.size() << " digrafo(s) desde JSON: " << ruta << "\n";
        return resultado;
    }
};

// ==========================================
// 5. CONTROLADOR DE MULTIPLES DIGRAFOS (POO + DIP)
// ==========================================
class GestorMultiDigrafos {
private:
    map<string, DigrafoCuaderno> digrafos;
    string digrafoActivo;
    unique_ptr<IPersistenciaColeccionDigrafos> persistencia;

public:
    GestorMultiDigrafos(unique_ptr<IPersistenciaColeccionDigrafos> pers)
        : persistencia(move(pers)) {}

    bool hayDigrafos() const {
        return !digrafos.empty();
    }

    string getDigrafoActivo() const {
        return digrafoActivo;
    }

    void listarDigrafos() const {
        if (digrafos.empty()) {
            cout << "\n[AVISO] No hay digrafos creados en memoria actualmente.\n";
            return;
        }
        cout << "\n--- DIGRAFOS DISPONIBLES ---\n";
        int i = 1;
        for (const auto& par : digrafos) {
            cout << i++ << ". " << par.first
                 << " (Vertices: " << par.second.getVerticesV().size()
                 << ", Arcos: " << par.second.getArcosA().size() << ")";
            if (par.first == digrafoActivo) cout << " [ACTIVO]";
            cout << "\n";
        }
    }

    void crearOSeleccionarDigrafo(const string& nombre) {
        if (digrafos.find(nombre) == digrafos.end()) {
            digrafos[nombre] = DigrafoCuaderno(nombre);
            cout << "\n[INFO] Digrafo creado y seleccionado: " << nombre << "\n";
        } else {
            cout << "\n[INFO] Digrafo seleccionado como activo: " << nombre << "\n";
        }
        digrafoActivo = nombre;
    }

    void eliminarDigrafo(const string& nombre) {
        auto it = digrafos.find(nombre);
        if (it == digrafos.end()) {
            cout << "\n[ERROR] No existe ningun digrafo con el nombre: " << nombre << "\n";
            return;
        }
        digrafos.erase(it);
        cout << "\n[INFO] Digrafo eliminado: " << nombre << "\n";

        if (digrafoActivo == nombre) {
            digrafoActivo = digrafos.empty() ? "" : digrafos.begin()->first;
            cout << "[INFO] Digrafo activo actual: "
                 << (digrafoActivo.empty() ? "Ninguno" : digrafoActivo) << "\n";
        }
    }

    bool validarActivo() const {
        if (digrafoActivo.empty() || digrafos.find(digrafoActivo) == digrafos.end()) {
            cout << "\n[AVISO] Primero crea o selecciona un digrafo en la opcion 1.\n";
            return false;
        }
        return true;
    }

    void nuevoVertice(const string& v) {
        if (!validarActivo()) return;
        digrafos[digrafoActivo].agregarVertice(v);
        cout << "\n[INFO] Vertice agregado a V en " << digrafoActivo << ".\n";
    }

    void quitarVertice(const string& v) {
        if (!validarActivo()) return;
        if (digrafos[digrafoActivo].eliminarVertice(v)) {
            cout << "\n[INFO] Vertice eliminado de V en " << digrafoActivo << ".\n";
        } else {
            cout << "\n[ERROR] El vertice no existe en V.\n";
        }
    }

    void nuevoArco(const string& ni, const string& nf, const string& a, int tiempo, double cost) {
        if (!validarActivo()) return;
        digrafos[digrafoActivo].agregarArcoDirigido(ni, nf, a, tiempo, cost);
        cout << "\n[INFO] Arco dirigido (" << ni << " -> " << nf << ") agregado a A en " << digrafoActivo << ".\n";
    }

    void listarArcosActivo() const {
        if (!validarActivo()) return;
        digrafos.at(digrafoActivo).listarArcos();
    }

    size_t cantidadArcosActivo() const {
        if (!validarActivo()) return 0;
        return digrafos.at(digrafoActivo).getArcosA().size();
    }

    void modificarArco(size_t indice, const string& ni, const string& nf, const string& a, int tiempo, double cost) {
        if (!validarActivo()) return;
        if (digrafos[digrafoActivo].editarArco(indice, ni, nf, a, tiempo, cost)) {
            cout << "\n[INFO] Arco dirigido actualizado correctamente.\n";
        } else {
            cout << "\n[ERROR] Numero de arco no valido.\n";
        }
    }

    void quitarArco(size_t indice) {
        if (!validarActivo()) return;
        if (digrafos[digrafoActivo].eliminarArco(indice)) {
            cout << "\n[INFO] Arco dirigido eliminado de A.\n";
        } else {
            cout << "\n[ERROR] Numero de arco no valido.\n";
        }
    }

    void nuevoArcoPath(const string& ni, const string& nf, const string& a, int tiempo, double cost) {
        if (!validarActivo()) return;
        digrafos[digrafoActivo].agregarAPath(ni, nf, a, tiempo, cost);
        cout << "\n[INFO] Arco agregado a path en " << digrafoActivo << ".\n";
    }

    void mostrarDigrafoElegido(const string& nombre) const {
        auto it = digrafos.find(nombre);
        if (it == digrafos.end()) {
            cout << "\n[ERROR] No existe ningun digrafo con el nombre: " << nombre << "\n";
            return;
        }
        it->second.mostrarEstructura();
    }

    void guardarJSON(const string& ruta) {
        if (digrafos.empty()) {
            cout << "\n[AVISO] No hay digrafos para guardar.\n";
            return;
        }
        persistencia->guardarTodos(ruta, digrafos);
    }

    void cargarJSON(const string& ruta) {
        auto cargados = persistencia->cargarTodos(ruta);
        if (!cargados.empty()) {
            digrafos = cargados;
            digrafoActivo = digrafos.begin()->first;
            cout << "[INFO] Digrafo activo establecido en: " << digrafoActivo << "\n";
        }
    }
};

namespace {
    int leerEntero(const string& prompt) {
        int val;
        while (true) {
            cout << prompt;
            if (cin >> val) return val;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[ERROR] Ingresa un numero entero valido.\n";
        }
    }

    double leerDouble(const string& prompt) {
        double val;
        while (true) {
            cout << prompt;
            if (cin >> val) return val;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[ERROR] Ingresa un numero valido.\n";
        }
    }

    string leerToken(const string& prompt) {
        string token;
        cout << prompt;
        cin >> token;
        return token;
    }
}

// ==========================================
// 6. MENU INTERACTIVO
// ==========================================
int main() {
    GestorMultiDigrafos app(make_unique<ManejadorDigrafoJSON>());
    string archivoJSON = "digrafos_cuaderno.json";
    int opcion;

    do {
        cout << "\n=========================================================\n";
        cout << "    MENU - GESTION DE MULTIPLES DIGRAFOS Y PATH (JSON)   \n";
        cout << "    Digrafo activo: " << (app.getDigrafoActivo().empty() ? "Ninguno" : app.getDigrafoActivo()) << "\n";
        cout << "=========================================================\n";
        cout << "1. Crear o seleccionar digrafo activo\n";
        cout << "2. Agregar vertice a V (en digrafo activo)\n";
        cout << "3. Agregar arco dirigido a A (ni -> nf, a, tiempo, cost)\n";
        cout << "4. Agregar arco a path (list<Arco>)\n";
        cout << "5. Elegir que digrafo quieres ver\n";
        cout << "6. Guardar todos los digrafos en archivo JSON\n";
        cout << "7. Cargar digrafos desde archivo JSON\n";
        cout << "8. Editar elemento existente en memoria RAM\n";
        cout << "9. Eliminar elemento existente en memoria RAM\n";
        cout << "10. Salir\n";
        opcion = leerEntero("Selecciona una opcion: ");

        switch (opcion) {
            case 1: {
                app.listarDigrafos();
                string nombre = leerToken("Escribe el nombre del digrafo a crear o activar: ");
                app.crearOSeleccionarDigrafo(nombre);
                break;
            }
            case 2: {
                if (!app.validarActivo()) break;
                int cant = leerEntero("Cuantos vertices deseas agregar?: ");
                if (cant < 0) cant = 0;
                for (int i = 0; i < cant; ++i) {
                    string v = leerToken("Vertice [" + to_string(i + 1) + "] (sin espacios): ");
                    app.nuevoVertice(v);
                }
                break;
            }
            case 3: {
                if (!app.validarActivo()) break;
                int cant = leerEntero("Cuantos arcos dirigidos deseas agregar a A?: ");
                if (cant < 0) cant = 0;
                for (int i = 0; i < cant; ++i) {
                    string ni = leerToken("Arco [" + to_string(i + 1) + "] - ni (origen): ");
                    string nf = leerToken("Arco [" + to_string(i + 1) + "] - nf (destino): ");
                    string a = leerToken("a (etiqueta max 3 chars): ");
                    int tiempo = leerEntero("tiempo (int): ");
                    double cost = leerDouble("cost (double): ");
                    app.nuevoArco(ni, nf, a, tiempo, cost);
                }
                break;
            }
            case 4: {
                if (!app.validarActivo()) break;
                int cant = leerEntero("Cuantos arcos deseas agregar a path?: ");
                if (cant < 0) cant = 0;
                for (int i = 0; i < cant; ++i) {
                    string ni = leerToken("Paso [" + to_string(i + 1) + "] - ni (origen): ");
                    string nf = leerToken("Paso [" + to_string(i + 1) + "] - nf (destino): ");
                    string a = leerToken("a (etiqueta max 3 chars): ");
                    int tiempo = leerEntero("tiempo (int): ");
                    double cost = leerDouble("cost (double): ");
                    app.nuevoArcoPath(ni, nf, a, tiempo, cost);
                }
                break;
            }
            case 5: {
                if (!app.hayDigrafos()) {
                    cout << "\n[AVISO] No hay digrafos registrados para mostrar.\n";
                    break;
                }
                app.listarDigrafos();
                string elegido = leerToken("Que digrafo quieres ver? Escribe su nombre: ");
                app.mostrarDigrafoElegido(elegido);
                break;
            }
            case 6:
                app.guardarJSON(archivoJSON);
                break;
            case 7:
                app.cargarJSON(archivoJSON);
                break;
            case 8: {
                if (!app.validarActivo() || app.cantidadArcosActivo() == 0) {
                    cout << "\n[AVISO] No hay arcos dirigidos para editar en el digrafo activo.\n";
                    break;
                }
                app.listarArcosActivo();
                int num = leerEntero("Numero de arco a editar: ");
                if (num < 1 || static_cast<size_t>(num) > app.cantidadArcosActivo()) {
                    cout << "\n[ERROR] Numero de arco no valido.\n";
                    break;
                }

                string ni = leerToken("Nuevo ni (origen): ");
                string nf = leerToken("Nuevo nf (destino): ");
                string a = leerToken("Nueva etiqueta a (max 3 chars): ");
                int tiempo = leerEntero("Nuevo tiempo (int): ");
                double cost = leerDouble("Nuevo cost (double): ");
                app.modificarArco(static_cast<size_t>(num - 1), ni, nf, a, tiempo, cost);
                break;
            }
            case 9: {
                if (!app.hayDigrafos()) {
                    cout << "\n[AVISO] No hay digrafos para eliminar.\n";
                    break;
                }
                int tipo = leerEntero("Que deseas eliminar?\n1. Digrafo\n2. Vertice del digrafo activo\n3. Arco dirigido del digrafo activo\nSelecciona una opcion: ");

                if (tipo == 1) {
                    app.listarDigrafos();
                    string nombre = leerToken("Escribe el nombre del digrafo a eliminar: ");
                    app.eliminarDigrafo(nombre);
                } else if (tipo == 2) {
                    if (!app.validarActivo()) break;
                    string v = leerToken("Nombre del vertice a eliminar de V: ");
                    app.quitarVertice(v);
                } else if (tipo == 3) {
                    if (!app.validarActivo() || app.cantidadArcosActivo() == 0) {
                        cout << "\n[AVISO] No hay arcos dirigidos para eliminar en el digrafo activo.\n";
                        break;
                    }
                    app.listarArcosActivo();
                    int num = leerEntero("Numero de arco a eliminar: ");
                    if (num < 1 || static_cast<size_t>(num) > app.cantidadArcosActivo()) {
                        cout << "\n[ERROR] Numero de arco no valido.\n";
                        break;
                    }
                    app.quitarArco(static_cast<size_t>(num - 1));
                } else {
                    cout << "\n[ERROR] Opcion no valida.\n";
                }
                break;
            }
            case 10:
                cout << "\nSaliendo del programa. Hasta luego!\n";
                break;
            default:
                cout << "\n[ERROR] Opcion no valida.\n";
        }
    } while (opcion != 10);

    return 0;
}