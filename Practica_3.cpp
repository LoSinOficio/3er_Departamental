#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <cctype>

using namespace std;

// ==========================================
// 1. DATO / ELEMENTO COMPLEJO (SRP)
// Reutilizable para dato complejo y arreglo complejo
// ==========================================
class ItemComplejo {
public:
    string clave;
    int nivel;
    double peso;

    ItemComplejo(string c = "GEN", int n = 1, double p = 1.0)
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
        cout << "[" << clave << " N:" << nivel << " P:" << peso << "]";
    }
};

// ==========================================
// 2. TIPO DE DATO ABSTRACTO (ADT) - SRP
// Basico + arreglo simple + dato complejo + arreglo complejo
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
    static string limpiar(string s) {
        size_t ini = s.find_first_not_of(" \t\r\n");
        if (ini == string::npos) return "";
        size_t fin = s.find_last_not_of(" \t\r\n");
        return s.substr(ini, fin - ini + 1);
    }

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
        string resto = limpiar(linea.substr(dosPuntos + 1));
        if (!resto.empty() && resto.back() == ',') resto.pop_back();
        return limpiar(resto);
    }

public:
    void escribir(const string& ruta, const vector<shared_ptr<ElementoADT>>& elementos) override {
        ofstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "\n[ERROR] No se pudo abrir el archivo JSON para escritura.\n";
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
        cout << "\n[EXITO] Datos guardados correctamente en el archivo JSON: " << ruta << "\n";
    }

    vector<shared_ptr<ElementoADT>> leer(const string& ruta) override {
        vector<shared_ptr<ElementoADT>> elementos;
        ifstream archivo(ruta);

        if (!archivo.is_open()) {
            cout << "\n[AVISO] No se encontro un archivo JSON previo en la ruta. Se iniciara vacio.\n";
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
            string limp = limpiar(linea);
            if (limp.empty() || limp == "[" || limp == "]") continue;

            // Inicio de un objeto principal del arreglo raiz (indentacion de 4 espacios en formato generado)
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
                    if (!val.empty()) itemActual.nivel = stoi(val);
                } else if (limp.find("\"peso\"") != string::npos) {
                    string val = extraerValorDespuesDosPuntos(limp);
                    if (!val.empty()) itemActual.peso = stod(val);
                }
                continue;
            }

            if (limp.find("\"id\"") != string::npos) {
                string val = extraerValorDespuesDosPuntos(limp);
                if (!val.empty()) id = stoi(val);
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
                        string num = limpiar(tok);
                        if (!num.empty()) simples.push_back(stoi(num));
                    }
                }
            }

            // Cierre del objeto principal del elemento
            if (!enDatoComplejo && !enArreglosComplejos && !enObjetoComplejo && limp.find("}") != string::npos) {
                elementos.push_back(make_shared<ElementoADT>(id, nombre, simples, complejo, arreglosComplejos));
                enElemento = false;
            }
        }

        archivo.close();
        cout << "\n[EXITO] Datos leidos y recuperados desde el archivo JSON: " << ruta << "\n";
        return elementos;
    }
};

// ==========================================
// 5. CLASE GESTORA DEL SISTEMA (POO)
// ==========================================
class GestorSistema {
private:
    vector<shared_ptr<ElementoADT>> coleccionDatos;
    unique_ptr<IPersistenciaJSON> persistencia;

public:
    GestorSistema(unique_ptr<IPersistenciaJSON> pers) : persistencia(move(pers)) {}

    void agregarElemento(int id,
                         string nombre,
                         vector<int> simples,
                         ItemComplejo complejo,
                         vector<ItemComplejo> arreglosComplejos) {
        coleccionDatos.push_back(
            make_shared<ElementoADT>(id, nombre, simples, complejo, arreglosComplejos)
        );
        cout << "\n[INFO] Elemento agregado exitosamente a la memoria RAM.\n";
    }

    void guardarEnDisco(const string& ruta) {
        persistencia->escribir(ruta, coleccionDatos);
    }

    void cargarDesdeDisco(const string& ruta) {
        coleccionDatos = persistencia->leer(ruta);
    }

    void mostrarEnConsola() const {
        if (coleccionDatos.empty()) {
            cout << "\n[AVISO] No hay elementos registrados en la memoria actualmente.\n";
            return;
        }
        cout << "\n=======================================================================\n";
        cout << "                    REGISTROS ACTUALES EN MEMORIA                      \n";
        cout << "=======================================================================\n";
        for (const auto& elem : coleccionDatos) {
            cout << "ID: " << elem->getId()
                 << " | Nombre: " << elem->getNombre()
                 << " | Simples: [ ";
            for (int s : elem->getSimples()) {
                cout << s << " ";
            }
            cout << "]\n  Dato Complejo: ";
            elem->getComplejo().mostrar();
            cout << "\n  Arreglo Complejo: [ ";
            for (size_t i = 0; i < elem->getArreglosComplejos().size(); ++i) {
                elem->getArreglosComplejos()[i].mostrar();
                if (i + 1 < elem->getArreglosComplejos().size()) cout << ", ";
            }
            cout << " ]\n";
        }
        cout << "=======================================================================\n";
    }
};

// ==========================================
// 6. FUNCION PRINCIPAL CON MENU INTERACTIVO
// ==========================================
int main() {
    unique_ptr<IPersistenciaJSON> servicioJSON = make_unique<ManejadorJSON>();
    GestorSistema sistema(move(servicioJSON));

    int opcion;
    string nombreArchivo = "ejercicio_datos_completo.json";

    do {
        cout << "\n=========================================================\n";
        cout << "   MENU - ARCHIVOS JSON  \n";
        cout << "=========================================================\n";
        cout << "1. Agregar nuevo elemento (ADT)\n";
        cout << "2. Ver elementos en memoria RAM\n";
        cout << "3. Guardar elementos en archivo JSON (Disco)\n";
        cout << "4. Cargar elementos desde archivo JSON (Disco)\n";
        cout << "5. Salir\n";
        cout << "Selecciona una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                int id, cantSimples, cantComplejos;
                string nombre, cClave;
                int cNivel;
                double cPeso;

                cout << "Ingresa el ID numerico: ";
                cin >> id;
                cout << "Ingresa el Nombre o Descriptor: ";
                cin >> nombre;

                cout << "Cuantos valores tendra el arreglo simple?: ";
                cin >> cantSimples;
                vector<int> simples(cantSimples);
                for (int i = 0; i < cantSimples; ++i) {
                    cout << "  Valor simple [" << i + 1 << "]: ";
                    cin >> simples[i];
                }

                cout << "Dato complejo (clave nivel peso): ";
                cin >> cClave >> cNivel >> cPeso;
                ItemComplejo complejo(cClave, cNivel, cPeso);

                cout << "Cuantos objetos tendra el arreglo complejo?: ";
                cin >> cantComplejos;
                vector<ItemComplejo> arreglosComplejos;
                for (int i = 0; i < cantComplejos; ++i) {
                    string k;
                    int nv;
                    double ps;
                    cout << "  Item complejo [" << i + 1 << "] (clave nivel peso): ";
                    cin >> k >> nv >> ps;
                    arreglosComplejos.emplace_back(k, nv, ps);
                }

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
            case 5:
                cout << "\nSaliendo del programa. Hasta luego!\n";
                break;
            default:
                cout << "\n[ERROR] Opcion no valida. Intenta de nuevo.\n";
        }
    } while (opcion != 5);

    return 0;
}