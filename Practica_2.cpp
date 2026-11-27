#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>

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

    string aTXT() const {
        return clave + ":" + to_string(nivel) + ":" + to_string(peso);
    }

    static ItemComplejo desdeTXT(const string& texto) {
        stringstream ss(texto);
        string c, n, p;
        getline(ss, c, ':');
        getline(ss, n, ':');
        getline(ss, p, ':');
        if (c.empty()) c = "GEN";
        int niv = n.empty() ? 1 : stoi(n);
        double pes = p.empty() ? 1.0 : stod(p);
        return ItemComplejo(c, niv, pes);
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

    // Campos separados por '|' y elementos internos por ',' o ';'
    string aTXT() const {
        string linea = to_string(id) + " | " + nombre + " | ";

        for (size_t i = 0; i < simples.size(); ++i) {
            linea += to_string(simples[i]);
            if (i + 1 < simples.size()) linea += ",";
        }

        linea += " | " + complejo.aTXT() + " | ";

        for (size_t i = 0; i < arreglosComplejos.size(); ++i) {
            linea += arreglosComplejos[i].aTXT();
            if (i + 1 < arreglosComplejos.size()) linea += ";";
        }

        return linea;
    }
};

// ==========================================
// 3. INTERFAZ DE PERSISTENCIA - OCP / DIP
// ==========================================
class IPersistenciaTXT {
public:
    virtual void escribir(const string& ruta, const vector<shared_ptr<ElementoADT>>& elementos) = 0;
    virtual vector<shared_ptr<ElementoADT>> leer(const string& ruta) = 0;
    virtual ~IPersistenciaTXT() = default;
};

// ==========================================
// 4. IMPLEMENTACION CONCRETA - LSP / SRP
// ==========================================
class ManejadorTXT : public IPersistenciaTXT {
private:
    static string limpiar(string s) {
        size_t ini = s.find_first_not_of(" \t\r\n");
        if (ini == string::npos) return "";
        size_t fin = s.find_last_not_of(" \t\r\n");
        return s.substr(ini, fin - ini + 1);
    }

    static vector<string> separar(const string& texto, char sep) {
        vector<string> partes;
        stringstream ss(texto);
        string tok;
        while (getline(ss, tok, sep)) {
            string limp = limpiar(tok);
            if (!limp.empty()) partes.push_back(limp);
        }
        return partes;
    }

public:
    void escribir(const string& ruta, const vector<shared_ptr<ElementoADT>>& elementos) override {
        ofstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "\n[ERROR] No se pudo abrir el archivo TXT para escritura.\n";
            return;
        }

        archivo << "--- REPORTE DE REGISTROS TXT ---\n";
        for (const auto& elem : elementos) {
            archivo << elem->aTXT() << "\n";
        }

        archivo.close();
        cout << "\n[EXITO] Datos guardados correctamente en el archivo TXT: " << ruta << "\n";
    }

    vector<shared_ptr<ElementoADT>> leer(const string& ruta) override {
        vector<shared_ptr<ElementoADT>> elementos;
        ifstream archivo(ruta);

        if (!archivo.is_open()) {
            cout << "\n[AVISO] No se encontro un archivo TXT previo en la ruta. Se iniciara vacio.\n";
            return elementos;
        }

        string linea;
        getline(archivo, linea); // Omitir cabecera

        while (getline(archivo, linea)) {
            string limp = limpiar(linea);
            if (limp.empty() || limp.find("---") != string::npos) continue;

            vector<string> campos = separar(limp, '|');
            if (campos.size() < 4) continue;

            int id = stoi(campos[0]);
            string nombre = campos[1];

            vector<int> simples;
            for (const auto& valStr : separar(campos[2], ',')) {
                simples.push_back(stoi(valStr));
            }

            ItemComplejo complejo = ItemComplejo::desdeTXT(campos[3]);

            vector<ItemComplejo> arreglosComplejos;
            if (campos.size() >= 5) {
                for (const auto& itemStr : separar(campos[4], ';')) {
                    arreglosComplejos.push_back(ItemComplejo::desdeTXT(itemStr));
                }
            }

            elementos.push_back(make_shared<ElementoADT>(id, nombre, simples, complejo, arreglosComplejos));
        }

        archivo.close();
        cout << "\n[EXITO] Datos leidos y recuperados desde el archivo TXT: " << ruta << "\n";
        return elementos;
    }
};

// ==========================================
// 5. CLASE GESTORA DEL SISTEMA (POO)
// ==========================================
class GestorSistema {
private:
    vector<shared_ptr<ElementoADT>> coleccionDatos;
    unique_ptr<IPersistenciaTXT> persistencia;

public:
    GestorSistema(unique_ptr<IPersistenciaTXT> pers) : persistencia(move(pers)) {}

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
    unique_ptr<IPersistenciaTXT> servicioTXT = make_unique<ManejadorTXT>();
    GestorSistema sistema(move(servicioTXT));

    int opcion;
    string nombreArchivo = "ejercicio_datos_completo.txt";

    do {
        cout << "\n=========================================================\n";
        cout << "   MENU - ARCHIVOS TXT  \n";
        cout << "=========================================================\n";
        cout << "1. Agregar nuevo elemento (ADT)\n";
        cout << "2. Ver elementos en memoria RAM\n";
        cout << "3. Guardar elementos en archivo TXT (Disco)\n";
        cout << "4. Cargar elementos desde archivo TXT (Disco)\n";
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