#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <cctype>
#include <limits>

using namespace std;

// ==========================================
// UTILIDAD DE PARSEO SEGURO (SRP)
// ==========================================
class UtilJSON {
public:
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
};

// ==========================================
// 1. DATO / ELEMENTO COMPLEJO DE VIAJE (SRP)
// Reutilizable para transporte/tramo principal y escalas/actividades
// ==========================================
class ItemComplejo {
public:
    string clave;
    int nivel;
    double peso;

    ItemComplejo(string c = "TRM-GEN", int n = 1, double p = 1.0)
        : clave(c), nivel(n), peso(p) {}

    string aJSON(int indent = 12) const {
        string pad(indent, ' ');
        string inner(indent + 4, ' ');
        return pad + "{\n" +
               inner + "\"clave\": \"" + clave + "\",\n" +
               inner + "\"nivel\": " + to_string(nivel) + ",\n" +
               inner + "\"peso\": " + to_string(peso) + "\n" +
               pad + "}";
    }

    void mostrar() const {
        cout << "[" << clave << " | Prioridad/Cat:" << nivel << " | Peso/Equipaje(kg):" << peso << "]";
    }
};

// ==========================================
// 2. TIPO DE DATO ABSTRACTO DE VIAJES (ADT) - SRP
// ID + Destino/Viaje + arreglo simple + dato complejo + arreglo complejo
// ==========================================
class ElementoADT {
private:
    int id;
    string nombre;
    vector<int> simples;
    ItemComplejo complejo;
    vector<ItemComplejo> arreglosComplejos;

public:
    ElementoADT(int id,
                string nombre,
                vector<int> s,
                ItemComplejo c,
                vector<ItemComplejo> ac)
        : id(id), nombre(nombre), simples(s), complejo(c), arreglosComplejos(ac) {}

    int getId() const { return id; }
    string getNombre() const { return nombre; }
    const vector<int>& getSimples() const { return simples; }
    const ItemComplejo& getComplejo() const { return complejo; }
    const vector<ItemComplejo>& getArreglosComplejos() const { return arreglosComplejos; }

    string aJSON() const {
        string json = "    {\n";
        json += "        \"id\": " + to_string(id) + ",\n";
        json += "        \"nombre\": \"" + nombre + "\",\n";

        json += "        \"simples\": [";
        for (size_t i = 0; i < simples.size(); ++i) {
            json += to_string(simples[i]);
            if (i + 1 < simples.size()) json += ", ";
        }
        json += "],\n";

        json += "        \"dato_complejo\":\n";
        json += complejo.aJSON(12) + ",\n";

        json += "        \"arreglos_complejos\": [\n";
        for (size_t i = 0; i < arreglosComplejos.size(); ++i) {
            json += arreglosComplejos[i].aJSON(12);
            if (i + 1 < arreglosComplejos.size()) json += ",";
            json += "\n";
        }
        json += "        ]\n";

        json += "    }";
        return json;
    }
};

// ==========================================
// 3. INTERFAZ DE PERSISTENCIA - OCP / DIP
// ==========================================
class IPersistenciaJSON {
public:
    virtual void escribir(const string& ruta, const vector<shared_ptr<ElementoADT>>& elementos) = 0;
    virtual vector<shared_ptr<ElementoADT>> leer(const string& ruta) = 0;
    virtual ~IPersistenciaJSON() = default;
};

// ==========================================
// 4. IMPLEMENTACION CONCRETA - LSP / SRP
// ==========================================
class ManejadorJSON : public IPersistenciaJSON {
private:
    static string extraerStringCampo(const string& linea) {
        size_t dosPuntos = linea.find(':');
        if (dosPuntos == string::npos) return "";
        size_t q1 = linea.find('"', dosPuntos);
        if (q1 == string::npos) return "";
        size_t q2 = linea.find('"', q1 + 1);
        if (q2 == string::npos) return "";
        return linea.substr(q1 + 1, q2 - q1 - 1);
    }

    static string extraerValorDespuesDosPuntos(const string& linea) {
        size_t dosPuntos = linea.find(':');
        if (dosPuntos == string::npos) return "";
        string resto = UtilJSON::limpiar(linea.substr(dosPuntos + 1));
        if (!resto.empty() && resto.back() == ',') resto.pop_back();
        return UtilJSON::limpiar(resto);
    }

public:
    void escribir(const string& ruta, const vector<shared_ptr<ElementoADT>>& elementos) override {
        ofstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "\n[ERROR] No se pudo abrir el archivo JSON de viajes para escritura.\n";
            return;
        }

        archivo << "[\n";
        for (size_t i = 0; i < elementos.size(); ++i) {
            archivo << elementos[i]->aJSON();
            if (i + 1 < elementos.size()) {
                archivo << ",";
            }
            archivo << "\n";
        }
        archivo << "]\n";

        archivo.close();
        cout << "\n[EXITO] Itinerarios/viajes guardados correctamente en el archivo JSON: " << ruta << "\n";
    }

    vector<shared_ptr<ElementoADT>> leer(const string& ruta) override {
        vector<shared_ptr<ElementoADT>> elementos;
        ifstream archivo(ruta);

        if (!archivo.is_open()) {
            cout << "\n[AVISO] No se encontro un archivo JSON previo de viajes. Se iniciara vacio.\n";
            return elementos;
        }

        string linea;
        int id = 0;
        string nombre;
        vector<int> simples;
        ItemComplejo complejo;
        vector<ItemComplejo> arreglosComplejos;
        ItemComplejo itemActual;

        bool enElemento = false;
        bool enDatoComplejo = false;
        bool enArreglosComplejos = false;
        bool enObjetoComplejo = false;

        while (getline(archivo, linea)) {
            string limp = UtilJSON::limpiar(linea);
            if (limp.empty() || limp == "[" || limp == "]") continue;

            if (!enElemento && limp.find("{") != string::npos) {
                id = 0;
                nombre.clear();
                simples.clear();
                complejo = ItemComplejo();
                arreglosComplejos.clear();
                enDatoComplejo = false;
                enArreglosComplejos = false;
                enObjetoComplejo = false;
                enElemento = true;
                continue;
            }

            if (!enElemento) continue;

            if (limp.find("\"dato_complejo\"") != string::npos) {
                enDatoComplejo = true;
                enArreglosComplejos = false;
                complejo = ItemComplejo();
                continue;
            }

            if (limp.find("\"arreglos_complejos\"") != string::npos) {
                enArreglosComplejos = true;
                enDatoComplejo = false;
                arreglosComplejos.clear();
                continue;
            }

            if (enArreglosComplejos && limp.find("]") != string::npos && !enObjetoComplejo) {
                enArreglosComplejos = false;
                continue;
            }

            if ((enDatoComplejo || enArreglosComplejos) && limp.find("{") != string::npos) {
                itemActual = ItemComplejo();
                enObjetoComplejo = true;
                continue;
            }

            if (enObjetoComplejo && limp.find("}") != string::npos) {
                if (enDatoComplejo) {
                    complejo = itemActual;
                    enDatoComplejo = false;
                } else if (enArreglosComplejos) {
                    arreglosComplejos.push_back(itemActual);
                }
                enObjetoComplejo = false;
                continue;
            }

            if (enObjetoComplejo) {
                if (limp.find("\"clave\"") != string::npos) {
                    itemActual.clave = extraerStringCampo(limp);
                } else if (limp.find("\"nivel\"") != string::npos) {
                    string val = extraerValorDespuesDosPuntos(limp);
                    int nvTmp = 0;
                    if (UtilJSON::parsearEnteroSeguro(val, nvTmp)) itemActual.nivel = nvTmp;
                } else if (limp.find("\"peso\"") != string::npos) {
                    string val = extraerValorDespuesDosPuntos(limp);
                    double psTmp = 0.0;
                    if (UtilJSON::parsearDoubleSeguro(val, psTmp)) itemActual.peso = psTmp;
                }
                continue;
            }

            if (limp.find("\"id\"") != string::npos) {
                string val = extraerValorDespuesDosPuntos(limp);
                int idTmp = 0;
                if (UtilJSON::parsearEnteroSeguro(val, idTmp)) id = idTmp;
            } else if (limp.find("\"nombre\"") != string::npos) {
                nombre = extraerStringCampo(limp);
            } else if (limp.find("\"simples\"") != string::npos) {
                simples.clear();
                size_t a1 = limp.find('[');
                size_t a2 = limp.find(']');
                if (a1 != string::npos && a2 != string::npos && a2 > a1) {
                    stringstream ss(limp.substr(a1 + 1, a2 - a1 - 1));
                    string tok;
                    while (getline(ss, tok, ',')) {
                        int sTmp = 0;
                        if (UtilJSON::parsearEnteroSeguro(tok, sTmp)) {
                            simples.push_back(sTmp);
                        }
                    }
                }
            }

            if (!enDatoComplejo && !enArreglosComplejos && !enObjetoComplejo && limp.find("}") != string::npos) {
                elementos.push_back(make_shared<ElementoADT>(id, nombre, simples, complejo, arreglosComplejos));
                enElemento = false;
            }
        }

        archivo.close();
        cout << "\n[EXITO] Datos de viajes leidos y recuperados desde el archivo JSON: " << ruta << "\n";
        return elementos;
    }
};

// ==========================================
// 5. CLASE GESTORA DEL SISTEMA DE VIAJES (POO)
// ==========================================
class GestorSistema {
private:
    vector<shared_ptr<ElementoADT>> coleccionDatos;
    unique_ptr<IPersistenciaJSON> persistencia;

public:
    GestorSistema(unique_ptr<IPersistenciaJSON> pers) : persistencia(move(pers)) {}

    bool estaVacia() const {
        return coleccionDatos.empty();
    }

    size_t cantidadElementos() const {
        return coleccionDatos.size();
    }

    void agregarElemento(int id,
                         string nombre,
                         vector<int> simples,
                         ItemComplejo complejo,
                         vector<ItemComplejo> arreglosComplejos) {
        coleccionDatos.push_back(
            make_shared<ElementoADT>(id, nombre, simples, complejo, arreglosComplejos)
        );
        cout << "\n[INFO] Viaje agregado exitosamente a la memoria RAM.\n";
    }

    bool editarElemento(size_t indice,
                        int id,
                        string nombre,
                        vector<int> simples,
                        ItemComplejo complejo,
                        vector<ItemComplejo> arreglosComplejos) {
        if (indice >= coleccionDatos.size()) return false;
        coleccionDatos[indice] = make_shared<ElementoADT>(
            id, nombre, simples, complejo, arreglosComplejos
        );
        cout << "\n[INFO] Viaje [" << (indice + 1) << "] actualizado en la memoria RAM.\n";
        return true;
    }

    bool eliminarElemento(size_t indice) {
        if (indice >= coleccionDatos.size()) return false;
        coleccionDatos.erase(coleccionDatos.begin() + static_cast<long>(indice));
        cout << "\n[INFO] Viaje [" << (indice + 1) << "] eliminado de la memoria RAM.\n";
        return true;
    }

    void guardarEnDisco(const string& ruta) {
        persistencia->escribir(ruta, coleccionDatos);
    }

    void cargarDesdeDisco(const string& ruta) {
        coleccionDatos = persistencia->leer(ruta);
    }

    void listarResumido() const {
        if (coleccionDatos.empty()) {
            cout << "\n[AVISO] No hay viajes registrados en la memoria actualmente.\n";
            return;
        }
        cout << "\n--- VIAJES EN MEMORIA RAM ---\n";
        for (size_t i = 0; i < coleccionDatos.size(); ++i) {
            cout << (i + 1) << ". ID: " << coleccionDatos[i]->getId()
                 << " | Destino/Viaje: " << coleccionDatos[i]->getNombre() << "\n";
        }
    }

    void mostrarEnConsola() const {
        if (coleccionDatos.empty()) {
            cout << "\n[AVISO] No hay viajes registrados en la memoria actualmente.\n";
            return;
        }
        cout << "\n=======================================================================\n";
        cout << "                    ITINERARIOS / VIAJES EN MEMORIA                    \n";
        cout << "=======================================================================\n";
        for (size_t idx = 0; idx < coleccionDatos.size(); ++idx) {
            const auto& elem = coleccionDatos[idx];
            cout << "[" << (idx + 1) << "] ID: " << elem->getId()
                 << " | Destino/Viaje: " << elem->getNombre()
                 << " | Simples (dias/pasajeros/codigos): [ ";
            for (int s : elem->getSimples()) {
                cout << s << " ";
            }
            cout << "]\n  Transporte/Tramo principal: ";
            elem->getComplejo().mostrar();
            cout << "\n  Escalas o actividades adicionales: [ ";
            for (size_t i = 0; i < elem->getArreglosComplejos().size(); ++i) {
                elem->getArreglosComplejos()[i].mostrar();
                if (i + 1 < elem->getArreglosComplejos().size()) cout << ", ";
            }
            cout << " ]\n";
        }
        cout << "=======================================================================\n";
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

    void capturarDatosElemento(int& id,
                               string& nombre,
                               vector<int>& simples,
                               ItemComplejo& complejo,
                               vector<ItemComplejo>& arreglosComplejos) {
        id = leerEntero("Ingresa el ID numerico del viaje: ");
        nombre = leerToken("Ingresa el Destino o Nombre del viaje (sin espacios): ");

        int cantSimples = leerEntero("Cuantos valores numericos simples tendra (ej. dias, noches, pasajeros)?: ");
        if (cantSimples < 0) cantSimples = 0;
        simples.assign(cantSimples, 0);
        for (int i = 0; i < cantSimples; ++i) {
            simples[i] = leerEntero("  Valor simple [" + to_string(i + 1) + "]: ");
        }

        cout << "Transporte o tramo principal (clave nivel peso):\n";
        string cClave = leerToken("  Clave (ej. VUELO-VB3676 o HOTEL): ");
        int cNivel = leerEntero("  Nivel/prioridad (entero): ");
        double cPeso = leerDouble("  Peso/equipaje en kg (decimal): ");
        complejo = ItemComplejo(cClave, cNivel, cPeso);

        int cantComplejos = leerEntero("Cuantas escalas, tramos o actividades adicionales tendra?: ");
        if (cantComplejos < 0) cantComplejos = 0;
        arreglosComplejos.clear();
        for (int i = 0; i < cantComplejos; ++i) {
            cout << "  Escala/actividad [" << i + 1 << "] (clave nivel peso):\n";
            string k = leerToken("    Clave: ");
            int nv = leerEntero("    Nivel/prioridad (entero): ");
            double ps = leerDouble("    Peso/equipaje en kg (decimal): ");
            arreglosComplejos.emplace_back(k, nv, ps);
        }
    }
}

// ==========================================
// 6. FUNCION PRINCIPAL CON MENU INTERACTIVO
// ==========================================
int main() {
    unique_ptr<IPersistenciaJSON> servicioJSON = make_unique<ManejadorJSON>();
    GestorSistema sistema(move(servicioJSON));

    int opcion;
    string nombreArchivo = "viajes_adt.json";

    do {
        cout << "\n=========================================================\n";
        cout << "        MENU - GESTOR DE VIAJES (ARCHIVOS JSON)          \n";
        cout << "=========================================================\n";
        cout << "1. Agregar nuevo viaje (ADT)\n";
        cout << "2. Ver viajes en memoria RAM\n";
        cout << "3. Guardar viajes en archivo JSON (Disco)\n";
        cout << "4. Cargar viajes desde archivo JSON (Disco)\n";
        cout << "5. Editar viaje existente en memoria RAM\n";
        cout << "6. Eliminar viaje existente en memoria RAM\n";
        cout << "7. Salir\n";
        opcion = leerEntero("Selecciona una opcion: ");

        switch (opcion) {
            case 1: {
                int id;
                string nombre;
                vector<int> simples;
                ItemComplejo complejo;
                vector<ItemComplejo> arreglosComplejos;
                capturarDatosElemento(id, nombre, simples, complejo, arreglosComplejos);
                sistema.agregarElemento(id, nombre, simples, complejo, arreglosComplejos);
                break;
            }
            case 2:
                sistema.mostrarEnConsola();
                break;
            case 3:
                sistema.guardarEnDisco(nombreArchivo);
                break;
            case 4:
                sistema.cargarDesdeDisco(nombreArchivo);
                break;
            case 5: {
                if (sistema.estaVacia()) {
                    cout << "\n[AVISO] No hay viajes en memoria RAM para editar.\n";
                    break;
                }
                sistema.listarResumido();
                int num = leerEntero("Selecciona el numero de viaje a editar: ");
                if (num < 1 || static_cast<size_t>(num) > sistema.cantidadElementos()) {
                    cout << "\n[ERROR] Numero de viaje fuera de rango.\n";
                    break;
                }
                int id;
                string nombre;
                vector<int> simples;
                ItemComplejo complejo;
                vector<ItemComplejo> arreglosComplejos;
                capturarDatosElemento(id, nombre, simples, complejo, arreglosComplejos);
                sistema.editarElemento(
                    static_cast<size_t>(num - 1), id, nombre, simples, complejo, arreglosComplejos
                );
                break;
            }
            case 6: {
                if (sistema.estaVacia()) {
                    cout << "\n[AVISO] No hay viajes en memoria RAM para eliminar.\n";
                    break;
                }
                sistema.listarResumido();
                int num = leerEntero("Selecciona el numero de viaje a eliminar: ");
                if (num < 1 || static_cast<size_t>(num) > sistema.cantidadElementos()) {
                    cout << "\n[ERROR] Numero de viaje fuera de rango.\n";
                    break;
                }
                sistema.eliminarElemento(static_cast<size_t>(num - 1));
                break;
            }
            case 7:
                cout << "\nSaliendo del programa. Hasta luego!\n";
                break;
            default:
                cout << "\n[ERROR] Opcion no valida. Intenta de nuevo.\n";
        }
    } while (opcion != 7);

    return 0;
}