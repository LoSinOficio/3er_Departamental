#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <memory>
#include <cstring>

using namespace std;

// ==========================================
// 1. NODO / TRAMO DE CONEXION (SRP)
// ni: origen, nf: destino, a: etiqueta[3], tiempo: int, cost: double
// ==========================================
class NodoTramo {
private:
    string ni;
    string nf;
    char a[4];
    int tiempo;
    double cost;

public:
    NodoTramo(const string& origen = "",
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
        cout << "(" << ni << ", " << nf
             << ", a:" << getA()
             << ", tiempo:" << tiempo
             << ", cost:" << cost << ")";
    }
};

// ==========================================
// 2. GRAFO G = (V, A) Y RUTA PATH (SRP)
// ==========================================
class GrafoCuaderno {
private:
    string nombreGrafo;
    vector<string> verticesV;
    vector<NodoTramo> aristasA;
    list<NodoTramo> path;

public:
    GrafoCuaderno(const string& nombre = "G1") : nombreGrafo(nombre) {}

    string getNombre() const { return nombreGrafo; }
    void setNombre(const string& nombre) { nombreGrafo = nombre; }

    void limpiar() {
        verticesV.clear();
        aristasA.clear();
        path.clear();
    }

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

    void agregarTramo(const string& ni, const string& nf, const string& a, int tiempo, double cost) {
        agregarVertice(ni);
        agregarVertice(nf);
        aristasA.emplace_back(ni, nf, a, tiempo, cost);
    }

    bool editarTramo(size_t indice, const string& ni, const string& nf, const string& a, int tiempo, double cost) {
        if (indice >= aristasA.size()) return false;
        agregarVertice(ni);
        agregarVertice(nf);
        aristasA[indice].setNi(ni);
        aristasA[indice].setNf(nf);
        aristasA[indice].setEtiqueta(a);
        aristasA[indice].setTiempo(tiempo);
        aristasA[indice].setCost(cost);
        return true;
    }

    bool eliminarTramo(size_t indice) {
        if (indice >= aristasA.size()) return false;
        aristasA.erase(aristasA.begin() + static_cast<long>(indice));
        return true;
    }

    void agregarAPath(const string& ni, const string& nf, const string& a, int tiempo, double cost) {
        agregarVertice(ni);
        agregarVertice(nf);
        path.emplace_back(ni, nf, a, tiempo, cost);
    }

    void limpiarPath() {
        path.clear();
    }

    const vector<string>& getVerticesV() const { return verticesV; }
    const vector<NodoTramo>& getAristasA() const { return aristasA; }
    const list<NodoTramo>& getPath() const { return path; }

    void cargarDatos(const vector<string>& v, const vector<NodoTramo>& a, const list<NodoTramo>& p) {
        verticesV = v;
        aristasA = a;
        path = p;
    }

    void listarTramos() const {
        if (aristasA.empty()) {
            cout << "\n[AVISO] No hay tramos registrados en " << nombreGrafo << ".\n";
            return;
        }
        cout << "\n--- TRAMOS EN [" << nombreGrafo << "] ---\n";
        for (size_t i = 0; i < aristasA.size(); ++i) {
            cout << i + 1 << ". ";
            aristasA[i].mostrar();
            cout << "\n";
        }
    }

    void mostrarEstructura() const {
        cout << "\n=======================================================================\n";
        cout << "              GRAFO [" << nombreGrafo << "] : G = (V, A) Y PATH              \n";
        cout << "=======================================================================\n";

        cout << "V = { ";
        for (size_t i = 0; i < verticesV.size(); ++i) {
            cout << verticesV[i];
            if (i + 1 < verticesV.size()) cout << ", ";
        }
        cout << " }\n";

        cout << "A = { ";
        for (size_t i = 0; i < aristasA.size(); ++i) {
            cout << aristasA[i].getA();
            if (i + 1 < aristasA.size()) cout << ", ";
        }
        cout << " }\n\n";

        cout << "Tramos registrados:\n";
        if (aristasA.empty()) {
            cout << "  Sin tramos registrados.\n";
        } else {
            for (const auto& tramo : aristasA) {
                cout << "  ";
                tramo.mostrar();
                cout << "\n";
            }
        }

        cout << "\nPath (list<Nodo>):\n";
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
class IPersistenciaColeccionGrafos {
public:
    virtual void guardarTodos(const string& ruta, const map<string, GrafoCuaderno>& grafos) = 0;
    virtual map<string, GrafoCuaderno> cargarTodos(const string& ruta) = 0;
    virtual ~IPersistenciaColeccionGrafos() = default;
};

// ==========================================
// 4. IMPLEMENTACION JSON - LSP / SRP
// ==========================================
class ManejadorColeccionJSON : public IPersistenciaColeccionGrafos {
private:
    static string limpiar(const string& s) {
        size_t ini = s.find_first_not_of(" \t\r\n");
        if (ini == string::npos) return "";
        size_t fin = s.find_last_not_of(" \t\r\n");
        return s.substr(ini, fin - ini + 1);
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

    static NodoTramo parsearObjetoTramo(const string& obj) {
        NodoTramo t;
        t.setNi(extraerValorCampoString(obj, "ni"));
        t.setNf(extraerValorCampoString(obj, "nf"));
        string a = extraerValorCampoString(obj, "a");
        string tiempoStr = extraerValorCampoNumero(obj, "tiempo");
        string costStr = extraerValorCampoNumero(obj, "cost");

        if (!a.empty()) t.setEtiqueta(a);
        if (!tiempoStr.empty()) t.setTiempo(stoi(tiempoStr));
        if (!costStr.empty()) t.setCost(stod(costStr));
        return t;
    }

public:
    void guardarTodos(const string& ruta, const map<string, GrafoCuaderno>& grafos) override {
        ofstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "\n[ERROR] No se pudo abrir el archivo JSON para escritura.\n";
            return;
        }

        archivo << "{\n";
        archivo << "  \"grafos\": [\n";

        size_t gIdx = 0;
        for (const auto& par : grafos) {
            const GrafoCuaderno& grafo = par.second;
            const auto& v = grafo.getVerticesV();
            const auto& a = grafo.getAristasA();
            const auto& p = grafo.getPath();

            archivo << "    {\n";
            archivo << "      \"nombre\": \"" << grafo.getNombre() << "\",\n";
            archivo << "      \"G\": {\n";

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

            archivo << "        \"tramos\": [\n";
            for (size_t i = 0; i < a.size(); ++i) {
                archivo << a[i].aJSON(12);
                if (i + 1 < a.size()) archivo << ",";
                archivo << "\n";
            }
            archivo << "        ],\n";

            archivo << "        \"path\": [\n";
            size_t pIdx = 0;
            for (const auto& tramo : p) {
                archivo << tramo.aJSON(12);
                if (pIdx + 1 < p.size()) archivo << ",";
                archivo << "\n";
                ++pIdx;
            }
            archivo << "        ]\n";

            archivo << "      }\n";
            archivo << "    }";
            if (gIdx + 1 < grafos.size()) archivo << ",";
            archivo << "\n";
            ++gIdx;
        }

        archivo << "  ]\n";
        archivo << "}\n";

        archivo.close();
        cout << "\n[EXITO] Todos los grafos fueron guardados en: " << ruta << "\n";
    }

    map<string, GrafoCuaderno> cargarTodos(const string& ruta) override {
        map<string, GrafoCuaderno> resultado;
        ifstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "\n[AVISO] No se encontro el archivo JSON: " << ruta << "\n";
            return resultado;
        }

        string linea;
        string nombreActual = "";
        vector<string> vActual;
        vector<NodoTramo> aActual;
        list<NodoTramo> pathActual;
        string seccion = "";
        bool enGrafo = false;

        while (getline(archivo, linea)) {
            string limp = limpiar(linea);
            if (limp.empty()) continue;

            if (limp.find("\"nombre\"") != string::npos) {
                nombreActual = extraerValorCampoString(limp, "nombre");
                vActual.clear();
                aActual.clear();
                pathActual.clear();
                seccion = "";
                enGrafo = true;
                continue;
            }

            if (!enGrafo) continue;

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

            if (limp.find("\"tramos\"") != string::npos) {
                seccion = "tramos";
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
                NodoTramo tramo = parsearObjetoTramo(limp);
                if (seccion == "tramos") aActual.push_back(tramo);
                else if (seccion == "path") pathActual.push_back(tramo);
                continue;
            }

            if (seccion.empty() && (limp == "}," || (limp == "}" && !nombreActual.empty()))) {
                if (!nombreActual.empty() && resultado.find(nombreActual) == resultado.end()) {
                    GrafoCuaderno g(nombreActual);
                    g.cargarDatos(vActual, aActual, pathActual);
                    resultado[nombreActual] = g;
                }
            }
        }

        archivo.close();
        cout << "\n[EXITO] Se cargaron " << resultado.size() << " grafo(s) desde JSON: " << ruta << "\n";
        return resultado;
    }
};

// ==========================================
// 5. CONTROLADOR DE MULTIPLES GRAFOS (POO + DIP)
// ==========================================
class GestorMultiGrafos {
private:
    map<string, GrafoCuaderno> grafos;
    string grafoActivo;
    unique_ptr<IPersistenciaColeccionGrafos> persistencia;

public:
    GestorMultiGrafos(unique_ptr<IPersistenciaColeccionGrafos> pers)
        : persistencia(move(pers)) {}

    bool hayGrafos() const {
        return !grafos.empty();
    }

    string getGrafoActivo() const {
        return grafoActivo;
    }

    void listarGrafos() const {
        if (grafos.empty()) {
            cout << "\n[AVISO] No hay grafos creados en memoria actualmente.\n";
            return;
        }
        cout << "\n--- GRAFOS DISPONIBLES ---\n";
        int i = 1;
        for (const auto& par : grafos) {
            cout << i++ << ". " << par.first
                 << " (Vertices: " << par.second.getVerticesV().size()
                 << ", Tramos: " << par.second.getAristasA().size() << ")";
            if (par.first == grafoActivo) cout << " [ACTIVO]";
            cout << "\n";
        }
    }

    void crearOSeleccionarGrafo(const string& nombre) {
        if (grafos.find(nombre) == grafos.end()) {
            grafos[nombre] = GrafoCuaderno(nombre);
            cout << "\n[INFO] Grafo creado y seleccionado: " << nombre << "\n";
        } else {
            cout << "\n[INFO] Grafo seleccionado como activo: " << nombre << "\n";
        }
        grafoActivo = nombre;
    }

    void eliminarGrafo(const string& nombre) {
        auto it = grafos.find(nombre);
        if (it == grafos.end()) {
            cout << "\n[ERROR] No existe ningun grafo con el nombre: " << nombre << "\n";
            return;
        }
        grafos.erase(it);
        cout << "\n[INFO] Grafo eliminado: " << nombre << "\n";

        if (grafoActivo == nombre) {
            grafoActivo = grafos.empty() ? "" : grafos.begin()->first;
            cout << "[INFO] Grafo activo actual: "
                 << (grafoActivo.empty() ? "Ninguno" : grafoActivo) << "\n";
        }
    }

    bool validarActivo() const {
        if (grafoActivo.empty() || grafos.find(grafoActivo) == grafos.end()) {
            cout << "\n[AVISO] Primero crea o selecciona un grafo en la opcion 1.\n";
            return false;
        }
        return true;
    }

    void nuevoVertice(const string& v) {
        if (!validarActivo()) return;
        grafos[grafoActivo].agregarVertice(v);
        cout << "\n[INFO] Vertice agregado a V en " << grafoActivo << ".\n";
    }

    void quitarVertice(const string& v) {
        if (!validarActivo()) return;
        if (grafos[grafoActivo].eliminarVertice(v)) {
            cout << "\n[INFO] Vertice eliminado de V en " << grafoActivo << ".\n";
        } else {
            cout << "\n[ERROR] El vertice no existe en V.\n";
        }
    }

    void nuevoTramo(const string& ni, const string& nf, const string& a, int tiempo, double cost) {
        if (!validarActivo()) return;
        grafos[grafoActivo].agregarTramo(ni, nf, a, tiempo, cost);
        cout << "\n[INFO] Tramo agregado a A en " << grafoActivo << ".\n";
    }

    void listarTramosActivo() const {
        if (!validarActivo()) return;
        grafos.at(grafoActivo).listarTramos();
    }

    size_t cantidadTramosActivo() const {
        if (!validarActivo()) return 0;
        return grafos.at(grafoActivo).getAristasA().size();
    }

    void modificarTramo(size_t indice, const string& ni, const string& nf, const string& a, int tiempo, double cost) {
        if (!validarActivo()) return;
        if (grafos[grafoActivo].editarTramo(indice, ni, nf, a, tiempo, cost)) {
            cout << "\n[INFO] Tramo actualizado correctamente.\n";
        } else {
            cout << "\n[ERROR] Numero de tramo no valido.\n";
        }
    }

    void quitarTramo(size_t indice) {
        if (!validarActivo()) return;
        if (grafos[grafoActivo].eliminarTramo(indice)) {
            cout << "\n[INFO] Tramo eliminado de A.\n";
        } else {
            cout << "\n[ERROR] Numero de tramo no valido.\n";
        }
    }

    void nuevoTramoPath(const string& ni, const string& nf, const string& a, int tiempo, double cost) {
        if (!validarActivo()) return;
        grafos[grafoActivo].agregarAPath(ni, nf, a, tiempo, cost);
        cout << "\n[INFO] Tramo agregado a path en " << grafoActivo << ".\n";
    }

    void mostrarGrafoElegido(const string& nombre) const {
        auto it = grafos.find(nombre);
        if (it == grafos.end()) {
            cout << "\n[ERROR] No existe ningun grafo con el nombre: " << nombre << "\n";
            return;
        }
        it->second.mostrarEstructura();
    }

    void limpiarPathActivo() {
        if (!validarActivo()) return;
        grafos[grafoActivo].limpiarPath();
        cout << "\n[INFO] Ruta path vaciada en " << grafoActivo << ".\n";
    }

    void guardarJSON(const string& ruta) {
        if (grafos.empty()) {
            cout << "\n[AVISO] No hay grafos para guardar.\n";
            return;
        }
        persistencia->guardarTodos(ruta, grafos);
    }

    void cargarJSON(const string& ruta) {
        auto cargados = persistencia->cargarTodos(ruta);
        if (!cargados.empty()) {
            grafos = cargados;
            grafoActivo = grafos.begin()->first;
            cout << "[INFO] Grafo activo establecido en: " << grafoActivo << "\n";
        }
    }
};

// ==========================================
// 6. MENU INTERACTIVO
// ==========================================
int main() {
    GestorMultiGrafos app(make_unique<ManejadorColeccionJSON>());
    string archivoJSON = "grafos_cuaderno.json";
    int opcion;

    do {
        cout << "\n=========================================================\n";
        cout << "     MENU - GESTION DE MULTIPLES GRAFOS Y PATH (JSON)    \n";
        cout << "     Grafo activo: " << (app.getGrafoActivo().empty() ? "Ninguno" : app.getGrafoActivo()) << "\n";
        cout << "=========================================================\n";
        cout << "1. Crear o seleccionar grafo activo\n";
        cout << "2. Eliminar un grafo\n";
        cout << "3. Agregar vertice a V (en grafo activo)\n";
        cout << "4. Eliminar vertice de V (en grafo activo)\n";
        cout << "5. Agregar tramo a A (ni, nf, a, tiempo, cost)\n";
        cout << "6. Editar tramo de A\n";
        cout << "7. Eliminar tramo de A\n";
        cout << "8. Agregar tramo a path (list<Nodo>)\n";
        cout << "9. Elegir que grafo quieres ver\n";
        cout << "10. Guardar todos los grafos en archivo JSON\n";
        cout << "11. Cargar grafos desde archivo JSON\n";
        cout << "12. Limpiar path del grafo activo\n";
        cout << "13. Salir\n";
        cout << "Selecciona una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                app.listarGrafos();
                string nombre;
                cout << "Escribe el nombre del grafo a crear o activar: ";
                cin >> nombre;
                app.crearOSeleccionarGrafo(nombre);
                break;
            }
            case 2: {
                if (!app.hayGrafos()) {
                    cout << "\n[AVISO] No hay grafos para eliminar.\n";
                    break;
                }
                app.listarGrafos();
                string nombre;
                cout << "Escribe el nombre del grafo a eliminar: ";
                cin >> nombre;
                app.eliminarGrafo(nombre);
                break;
            }
            case 3: {
                if (!app.validarActivo()) break;
                int cant;
                cout << "Cuantos vertices deseas agregar?: ";
                cin >> cant;
                for (int i = 0; i < cant; ++i) {
                    string v;
                    cout << "Vertice [" << i + 1 << "] (sin espacios): ";
                    cin >> v;
                    app.nuevoVertice(v);
                }
                break;
            }
            case 4: {
                if (!app.validarActivo()) break;
                string v;
                cout << "Nombre del vertice a eliminar de V: ";
                cin >> v;
                app.quitarVertice(v);
                break;
            }
            case 5: {
                if (!app.validarActivo()) break;
                int cant;
                cout << "Cuantos tramos deseas agregar a A?: ";
                cin >> cant;
                for (int i = 0; i < cant; ++i) {
                    string ni, nf, a;
                    int tiempo;
                    double cost;
                    cout << "Tramo [" << i + 1 << "] - ni (origen) y nf (destino): ";
                    cin >> ni >> nf;
                    cout << "a (etiqueta max 3 chars): ";
                    cin >> a;
                    cout << "tiempo (int) y cost (double): ";
                    cin >> tiempo >> cost;
                    app.nuevoTramo(ni, nf, a, tiempo, cost);
                }
                break;
            }
            case 6: {
                if (!app.validarActivo() || app.cantidadTramosActivo() == 0) {
                    cout << "\n[AVISO] No hay tramos para editar en el grafo activo.\n";
                    break;
                }
                app.listarTramosActivo();
                size_t num;
                cout << "Numero de tramo a editar: ";
                cin >> num;

                string ni, nf, a;
                int tiempo;
                double cost;
                cout << "Nuevo ni (origen) y nf (destino): ";
                cin >> ni >> nf;
                cout << "Nueva etiqueta a (max 3 chars): ";
                cin >> a;
                cout << "Nuevo tiempo (int) y cost (double): ";
                cin >> tiempo >> cost;
                app.modificarTramo(num - 1, ni, nf, a, tiempo, cost);
                break;
            }
            case 7: {
                if (!app.validarActivo() || app.cantidadTramosActivo() == 0) {
                    cout << "\n[AVISO] No hay tramos para eliminar en el grafo activo.\n";
                    break;
                }
                app.listarTramosActivo();
                size_t num;
                cout << "Numero de tramo a eliminar: ";
                cin >> num;
                app.quitarTramo(num - 1);
                break;
            }
            case 8: {
                if (!app.validarActivo()) break;
                int cant;
                cout << "Cuantos tramos deseas agregar a path?: ";
                cin >> cant;
                for (int i = 0; i < cant; ++i) {
                    string ni, nf, a;
                    int tiempo;
                    double cost;
                    cout << "Paso [" << i + 1 << "] - ni (origen) y nf (destino): ";
                    cin >> ni >> nf;
                    cout << "a (etiqueta max 3 chars): ";
                    cin >> a;
                    cout << "tiempo (int) y cost (double): ";
                    cin >> tiempo >> cost;
                    app.nuevoTramoPath(ni, nf, a, tiempo, cost);
                }
                break;
            }
            case 9: {
                if (!app.hayGrafos()) {
                    cout << "\n[AVISO] No hay grafos registrados para mostrar.\n";
                    break;
                }
                app.listarGrafos();
                string elegido;
                cout << "Que grafo quieres ver? Escribe su nombre: ";
                cin >> elegido;
                app.mostrarGrafoElegido(elegido);
                break;
            }
            case 10:
                app.guardarJSON(archivoJSON);
                break;
            case 11:
                app.cargarJSON(archivoJSON);
                break;
            case 12:
                app.limpiarPathActivo();
                break;
            case 13:
                cout << "\nSaliendo del programa. Hasta luego!\n";
                break;
            default:
                cout << "\n[ERROR] Opcion no valida.\n";
        }
    } while (opcion != 13);

    return 0;
}