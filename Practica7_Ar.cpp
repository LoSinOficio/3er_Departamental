#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <cstring>
#include <limits>

using namespace std;

// ==========================================
// 1. NODO DE ARBOL (SRP)
// valor: identificador/dato, etiqueta[3], peso: double, padre: string
// ==========================================
class NodoArbol {
private:
    string valor;
    string padre;
    char etiqueta[4];
    double peso;

public:
    NodoArbol(const string& val = "",
              const string& pad = "",
              const string& etq = "r",
              double p = 0.0)
        : valor(val), padre(pad), peso(p) {
        setEtiqueta(etq);
    }

    void setEtiqueta(const string& etq) {
        strncpy(etiqueta, etq.c_str(), 3);
        etiqueta[3] = '\0';
    }

    string getValor() const { return valor; }
    string getPadre() const { return padre; }
    string getEtiqueta() const { return string(etiqueta); }
    double getPeso() const { return peso; }

    void setValor(const string& val) { valor = val; }
    void setPadre(const string& pad) { padre = pad; }
    void setPeso(double p) { peso = p; }

    string aJSON(int indent = 12) const {
        string pad(indent, ' ');
        return pad + "{\"valor\": \"" + valor +
               "\", \"padre\": \"" + padre +
               "\", \"etiqueta\": \"" + getEtiqueta() +
               "\", \"peso\": " + to_string(peso) + "}";
    }

    void mostrar() const {
        if (padre.empty()) {
            cout << "[RAIZ] " << valor << " (etq:" << getEtiqueta() << ", peso:" << peso << ")";
        } else {
            cout << padre << " -> " << valor << " (etq:" << getEtiqueta() << ", peso:" << peso << ")";
        }
    }
};

// ==========================================
// 2. ARBOL T = (V, E) (SRP)
// ==========================================
class ArbolCuaderno {
private:
    string nombreArbol;
    string raiz;
    vector<NodoArbol> nodos;

    int buscarIndiceNodo(const string& val) const {
        for (size_t i = 0; i < nodos.size(); ++i) {
            if (nodos[i].getValor() == val) return static_cast<int>(i);
        }
        return -1;
    }

    void imprimirRama(const string& actual, const string& prefijo, bool esUltimo) const {
        int idx = buscarIndiceNodo(actual);
        if (idx == -1) return;

        cout << prefijo;
        cout << (esUltimo ? "\\-- " : "|-- ");
        cout << nodos[idx].getValor()
             << " [etq:" << nodos[idx].getEtiqueta()
             << ", peso:" << nodos[idx].getPeso() << "]\n";

        vector<string> hijos;
        for (const auto& n : nodos) {
            if (n.getPadre() == actual && n.getValor() != actual) {
                hijos.push_back(n.getValor());
            }
        }

        string nuevoPrefijo = prefijo + (esUltimo ? "    " : "|   ");
        for (size_t i = 0; i < hijos.size(); ++i) {
            imprimirRama(hijos[i], nuevoPrefijo, i + 1 == hijos.size());
        }
    }

    void recolectarDescendientes(const string& objetivo, vector<string>& porBorrar) const {
        porBorrar.push_back(objetivo);
        for (const auto& n : nodos) {
            if (n.getPadre() == objetivo && n.getValor() != objetivo) {
                recolectarDescendientes(n.getValor(), porBorrar);
            }
        }
    }

public:
    ArbolCuaderno(const string& nombre = "T1") : nombreArbol(nombre), raiz("") {}

    string getNombre() const { return nombreArbol; }
    void setNombre(const string& nombre) { nombreArbol = nombre; }
    string getRaiz() const { return raiz; }

    bool existeNodo(const string& val) const {
        return buscarIndiceNodo(val) != -1;
    }

    bool establecerRaiz(const string& val, const string& etq = "rai", double peso = 0.0) {
        if (val.empty()) return false;
        if (!nodos.empty()) {
            return false;
        }
        raiz = val;
        nodos.emplace_back(val, "", etq, peso);
        return true;
    }

    bool agregarHijo(const string& padre, const string& hijo, const string& etq = "r", double peso = 0.0) {
        if (hijo.empty() || existeNodo(hijo)) return false;
        if (nodos.empty()) {
            raiz = hijo;
            nodos.emplace_back(hijo, "", etq, peso);
            return true;
        }
        if (!existeNodo(padre)) return false;
        nodos.emplace_back(hijo, padre, etq, peso);
        return true;
    }

    bool editarNodo(const string& actual, const string& nuevoValor, const string& etq, double peso) {
        int idx = buscarIndiceNodo(actual);
        if (idx == -1 || nuevoValor.empty()) return false;
        if (actual != nuevoValor && existeNodo(nuevoValor)) return false;

        for (auto& n : nodos) {
            if (n.getPadre() == actual) {
                n.setPadre(nuevoValor);
            }
        }
        if (raiz == actual) {
            raiz = nuevoValor;
        }
        nodos[idx].setValor(nuevoValor);
        nodos[idx].setEtiqueta(etq);
        nodos[idx].setPeso(peso);
        return true;
    }

    bool eliminarNodoYSubarbol(const string& val) {
        int idx = buscarIndiceNodo(val);
        if (idx == -1) return false;

        vector<string> porBorrar;
        recolectarDescendientes(val, porBorrar);

        vector<NodoArbol> restantes;
        for (const auto& n : nodos) {
            bool borrar = false;
            for (const auto& b : porBorrar) {
                if (n.getValor() == b) {
                    borrar = true;
                    break;
                }
            }
            if (!borrar) restantes.push_back(n);
        }
        nodos = restantes;

        if (raiz == val) {
            raiz = nodos.empty() ? "" : nodos.front().getValor();
            if (!raiz.empty()) {
                nodos.front().setPadre("");
            }
        }
        return true;
    }

    const vector<NodoArbol>& getNodos() const { return nodos; }

    void cargarDatos(const string& r, const vector<NodoArbol>& listaNodos) {
        raiz = r;
        nodos = listaNodos;
        if (raiz.empty() && !nodos.empty()) {
            raiz = nodos.front().getValor();
        }
    }

    void listarNodos() const {
        if (nodos.empty()) {
            cout << "\n[AVISO] El arbol [" << nombreArbol << "] esta vacio.\n";
            return;
        }
        cout << "\n--- NODOS EN [" << nombreArbol << "] ---\n";
        for (size_t i = 0; i < nodos.size(); ++i) {
            cout << i + 1 << ". ";
            nodos[i].mostrar();
            cout << "\n";
        }
    }

    void mostrarEstructura() const {
        cout << "\n=======================================================================\n";
        cout << "                 ARBOL [" << nombreArbol << "] : T = (V, E)                  \n";
        cout << "=======================================================================\n";

        cout << "V = { ";
        for (size_t i = 0; i < nodos.size(); ++i) {
            cout << nodos[i].getValor();
            if (i + 1 < nodos.size()) cout << ", ";
        }
        cout << " }\n";

        cout << "Raiz: " << (raiz.empty() ? "(sin raiz)" : raiz)
             << " | Total de nodos: " << nodos.size() << "\n\n";

        if (nodos.empty()) {
            cout << "  Arbol vacio.\n";
        } else {
            cout << "Estructura jerarquica:\n";
            string raizMostrar = existeNodo(raiz) ? raiz : nodos.front().getValor();
            imprimirRama(raizMostrar, "  ", true);
        }
        cout << "=======================================================================\n";
    }
};

// ==========================================
// 3. INTERFAZ DE PERSISTENCIA - OCP / DIP
// ==========================================
class IPersistenciaColeccionArboles {
public:
    virtual void guardarTodos(const string& ruta, const map<string, ArbolCuaderno>& arboles) = 0;
    virtual map<string, ArbolCuaderno> cargarTodos(const string& ruta) = 0;
    virtual ~IPersistenciaColeccionArboles() = default;
};

// ==========================================
// 4. IMPLEMENTACION JSON - LSP / SRP
// ==========================================
class ManejadorArbolJSON : public IPersistenciaColeccionArboles {
private:
    static string limpiar(const string& s) {
        size_t ini = s.find_first_not_of(" \t\r\n");
        if (ini == string::npos) return "";
        size_t fin = s.find_last_not_of(" \t\r\n");
        return s.substr(ini, fin - ini + 1);
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

    static NodoArbol parsearObjetoNodo(const string& obj) {
        NodoArbol n;
        n.setValor(extraerValorCampoString(obj, "valor"));
        n.setPadre(extraerValorCampoString(obj, "padre"));
        string etq = extraerValorCampoString(obj, "etiqueta");
        string pesoStr = extraerValorCampoNumero(obj, "peso");

        double pesoVal = 0.0;
        if (!etq.empty()) n.setEtiqueta(etq);
        if (parsearDoubleSeguro(pesoStr, pesoVal)) n.setPeso(pesoVal);
        return n;
    }

public:
    void guardarTodos(const string& ruta, const map<string, ArbolCuaderno>& arboles) override {
        ofstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "\n[ERROR] No se pudo abrir el archivo JSON para escritura.\n";
            return;
        }

        archivo << "{\n";
        archivo << "  \"arboles\": [\n";

        size_t aIdx = 0;
        for (const auto& par : arboles) {
            const ArbolCuaderno& arbol = par.second;
            const auto& lista = arbol.getNodos();

            archivo << "    {\n";
            archivo << "      \"nombre\": \"" << arbol.getNombre() << "\",\n";
            archivo << "      \"T\": {\n";
            archivo << "        \"raiz\": \"" << arbol.getRaiz() << "\",\n";
            archivo << "        \"nodos\": [\n";

            for (size_t i = 0; i < lista.size(); ++i) {
                archivo << lista[i].aJSON(12);
                if (i + 1 < lista.size()) archivo << ",";
                archivo << "\n";
            }

            archivo << "        ]\n";
            archivo << "      }\n";
            archivo << "    }";
            if (aIdx + 1 < arboles.size()) archivo << ",";
            archivo << "\n";
            ++aIdx;
        }

        archivo << "  ]\n";
        archivo << "}\n";

        archivo.close();
        cout << "\n[EXITO] Todos los arboles fueron guardados en: " << ruta << "\n";
    }

    map<string, ArbolCuaderno> cargarTodos(const string& ruta) override {
        map<string, ArbolCuaderno> resultado;
        ifstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "\n[AVISO] No se encontro el archivo JSON: " << ruta << "\n";
            return resultado;
        }

        string linea;
        string nombreActual = "";
        string raizActual = "";
        vector<NodoArbol> nodosActuales;
        bool enNodos = false;
        bool enArbol = false;

        while (getline(archivo, linea)) {
            string limp = limpiar(linea);
            if (limp.empty()) continue;

            if (limp.find("\"nombre\"") != string::npos) {
                nombreActual = extraerValorCampoString(limp, "nombre");
                raizActual.clear();
                nodosActuales.clear();
                enNodos = false;
                enArbol = true;
                continue;
            }

            if (!enArbol) continue;

            if (limp.find("\"raiz\"") != string::npos) {
                raizActual = extraerValorCampoString(limp, "raiz");
                continue;
            }

            if (limp.find("\"nodos\"") != string::npos) {
                enNodos = true;
                continue;
            }

            if (enNodos && limp.find("]") != string::npos && limp.find("{") == string::npos) {
                enNodos = false;
                continue;
            }

            if (enNodos && limp.find("{") != string::npos) {
                nodosActuales.push_back(parsearObjetoNodo(limp));
                continue;
            }

            if (!enNodos && (limp == "}," || (limp == "}" && !nombreActual.empty()))) {
                if (!nombreActual.empty() && resultado.find(nombreActual) == resultado.end()) {
                    ArbolCuaderno t(nombreActual);
                    t.cargarDatos(raizActual, nodosActuales);
                    resultado[nombreActual] = t;
                }
            }
        }

        archivo.close();
        cout << "\n[EXITO] Se cargaron " << resultado.size() << " arbol(es) desde JSON: " << ruta << "\n";
        return resultado;
    }
};

// ==========================================
// 5. CONTROLADOR DE MULTIPLES ARBOLES (POO + DIP)
// ==========================================
class GestorMultiArboles {
private:
    map<string, ArbolCuaderno> arboles;
    string arbolActivo;
    unique_ptr<IPersistenciaColeccionArboles> persistencia;

public:
    GestorMultiArboles(unique_ptr<IPersistenciaColeccionArboles> pers)
        : persistencia(move(pers)) {}

    bool hayArboles() const {
        return !arboles.empty();
    }

    string getArbolActivo() const {
        return arbolActivo;
    }

    void listarArboles() const {
        if (arboles.empty()) {
            cout << "\n[AVISO] No hay arboles creados en memoria actualmente.\n";
            return;
        }
        cout << "\n--- ARBOLES DISPONIBLES ---\n";
        int i = 1;
        for (const auto& par : arboles) {
            cout << i++ << ". " << par.first
                 << " (Raiz: " << (par.second.getRaiz().empty() ? "ninguna" : par.second.getRaiz())
                 << ", Nodos: " << par.second.getNodos().size() << ")";
            if (par.first == arbolActivo) cout << " [ACTIVO]";
            cout << "\n";
        }
    }

    void crearOSeleccionarArbol(const string& nombre) {
        if (arboles.find(nombre) == arboles.end()) {
            arboles[nombre] = ArbolCuaderno(nombre);
            cout << "\n[INFO] Arbol creado y seleccionado: " << nombre << "\n";
        } else {
            cout << "\n[INFO] Arbol seleccionado como activo: " << nombre << "\n";
        }
        arbolActivo = nombre;
    }

    void eliminarArbol(const string& nombre) {
        auto it = arboles.find(nombre);
        if (it == arboles.end()) {
            cout << "\n[ERROR] No existe ningun arbol con el nombre: " << nombre << "\n";
            return;
        }
        arboles.erase(it);
        cout << "\n[INFO] Arbol eliminado: " << nombre << "\n";

        if (arbolActivo == nombre) {
            arbolActivo = arboles.empty() ? "" : arboles.begin()->first;
            cout << "[INFO] Arbol activo actual: "
                 << (arbolActivo.empty() ? "Ninguno" : arbolActivo) << "\n";
        }
    }

    bool validarActivo() const {
        if (arbolActivo.empty() || arboles.find(arbolActivo) == arboles.end()) {
            cout << "\n[AVISO] Primero crea o selecciona un arbol en la opcion 1.\n";
            return false;
        }
        return true;
    }

    void ponerRaiz(const string& val, const string& etq, double peso) {
        if (!validarActivo()) return;
        if (arboles[arbolActivo].establecerRaiz(val, etq, peso)) {
            cout << "\n[INFO] Raiz '" << val << "' establecida en " << arbolActivo << ".\n";
        } else {
            cout << "\n[ERROR] El arbol ya tiene nodos o el valor esta vacio.\n";
        }
    }

    void insertarHijo(const string& padre, const string& hijo, const string& etq, double peso) {
        if (!validarActivo()) return;
        if (arboles[arbolActivo].agregarHijo(padre, hijo, etq, peso)) {
            cout << "\n[INFO] Nodo '" << hijo << "' agregado en " << arbolActivo << ".\n";
        } else {
            cout << "\n[ERROR] Verifica que el nodo padre exista y que el nuevo nodo no este repetido.\n";
        }
    }

    void listarNodosActivo() const {
        if (!validarActivo()) return;
        arboles.at(arbolActivo).listarNodos();
    }

    void modificarNodo(const string& actual, const string& nuevoValor, const string& etq, double peso) {
        if (!validarActivo()) return;
        if (arboles[arbolActivo].editarNodo(actual, nuevoValor, etq, peso)) {
            cout << "\n[INFO] Nodo actualizado correctamente.\n";
        } else {
            cout << "\n[ERROR] No se pudo editar el nodo (revisa si existe o si el nuevo nombre ya existe).\n";
        }
    }

    void quitarNodoYSubarbol(const string& val) {
        if (!validarActivo()) return;
        if (arboles[arbolActivo].eliminarNodoYSubarbol(val)) {
            cout << "\n[INFO] Nodo '" << val << "' y sus descendientes eliminados.\n";
        } else {
            cout << "\n[ERROR] El nodo no existe en el arbol activo.\n";
        }
    }

    void mostrarArbolElegido(const string& nombre) const {
        auto it = arboles.find(nombre);
        if (it == arboles.end()) {
            cout << "\n[ERROR] No existe ningun arbol con el nombre: " << nombre << "\n";
            return;
        }
        it->second.mostrarEstructura();
    }

    void guardarJSON(const string& ruta) {
        if (arboles.empty()) {
            cout << "\n[AVISO] No hay arboles para guardar.\n";
            return;
        }
        persistencia->guardarTodos(ruta, arboles);
    }

    void cargarJSON(const string& ruta) {
        auto cargados = persistencia->cargarTodos(ruta);
        if (!cargados.empty()) {
            arboles = cargados;
            arbolActivo = arboles.begin()->first;
            cout << "[INFO] Arbol activo establecido en: " << arbolActivo << "\n";
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
    GestorMultiArboles app(make_unique<ManejadorArbolJSON>());
    string archivoJSON = "arboles_cuaderno.json";
    int opcion;

    do {
        cout << "\n=========================================================\n";
        cout << "      MENU - GESTION DE MULTIPLES ARBOLES (JSON)         \n";
        cout << "      Arbol activo: " << (app.getArbolActivo().empty() ? "Ninguno" : app.getArbolActivo()) << "\n";
        cout << "=========================================================\n";
        cout << "1. Crear o seleccionar arbol activo\n";
        cout << "2. Establecer nodo raiz (si el arbol esta vacio)\n";
        cout << "3. Agregar nodo hijo a un padre existente\n";
        cout << "4. Elegir que arbol quieres ver\n";
        cout << "5. Guardar todos los arboles en archivo JSON\n";
        cout << "6. Cargar arboles desde archivo JSON\n";
        cout << "7. Editar elemento existente en memoria RAM\n";
        cout << "8. Eliminar elemento existente en memoria RAM\n";
        cout << "9. Salir\n";
        opcion = leerEntero("Selecciona una opcion: ");

        switch (opcion) {
            case 1: {
                app.listarArboles();
                string nombre = leerToken("Escribe el nombre del arbol a crear o activar: ");
                app.crearOSeleccionarArbol(nombre);
                break;
            }
            case 2: {
                if (!app.validarActivo()) break;
                string val = leerToken("Valor/nombre de la raiz (sin espacios): ");
                string etq = leerToken("Etiqueta (max 3 chars): ");
                double peso = leerDouble("Peso inicial (double): ");
                app.ponerRaiz(val, etq, peso);
                break;
            }
            case 3: {
                if (!app.validarActivo()) break;
                string padre = leerToken("Nodo padre existente: ");
                string hijo = leerToken("Nuevo nodo hijo (sin espacios): ");
                string etq = leerToken("Etiqueta de la rama (max 3 chars): ");
                double peso = leerDouble("Peso de la rama (double): ");
                app.insertarHijo(padre, hijo, etq, peso);
                break;
            }
            case 4: {
                if (!app.hayArboles()) {
                    cout << "\n[AVISO] No hay arboles registrados para mostrar.\n";
                    break;
                }
                app.listarArboles();
                string elegido = leerToken("Que arbol quieres ver? Escribe su nombre: ");
                app.mostrarArbolElegido(elegido);
                break;
            }
            case 5:
                app.guardarJSON(archivoJSON);
                break;
            case 6:
                app.cargarJSON(archivoJSON);
                break;
            case 7: {
                if (!app.validarActivo()) break;
                app.listarNodosActivo();
                string actual = leerToken("Valor del nodo a editar: ");
                string nuevoVal = leerToken("Nuevo valor del nodo: ");
                string etq = leerToken("Nueva etiqueta (max 3 chars): ");
                double peso = leerDouble("Nuevo peso (double): ");
                app.modificarNodo(actual, nuevoVal, etq, peso);
                break;
            }
            case 8: {
                if (!app.hayArboles()) {
                    cout << "\n[AVISO] No hay arboles para eliminar.\n";
                    break;
                }
                int tipo = leerEntero("Que deseas eliminar?\n1. Arbol completo\n2. Nodo y su subarbol (en arbol activo)\nSelecciona una opcion: ");
                if (tipo == 1) {
                    app.listarArboles();
                    string nombre = leerToken("Escribe el nombre del arbol a eliminar: ");
                    app.eliminarArbol(nombre);
                } else if (tipo == 2) {
                    if (!app.validarActivo()) break;
                    app.listarNodosActivo();
                    string val = leerToken("Valor del nodo a eliminar (junto con sus hijos): ");
                    app.quitarNodoYSubarbol(val);
                } else {
                    cout << "\n[ERROR] Opcion no valida.\n";
                }
                break;
            }
            case 9:
                cout << "\nSaliendo del programa. Hasta luego!\n";
                break;
            default:
                cout << "\n[ERROR] Opcion no valida.\n";
        }
    } while (opcion != 9);

    return 0;
}