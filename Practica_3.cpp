#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>

using namespace std;

// ==========================================
// 1. TIPO DE DATO ABSTRACTO (ADT) - SRP
// ==========================================
class ElementoADT {
private:
    int id;                     
    string nombre;              
    vector<int> calificaciones; 

public:
    ElementoADT(int id, string nombre, vector<int> cals) 
        : id(id), nombre(nombre), calificaciones(cals) {}

    int getId() const { return id; }
    string getNombre() const { return nombre; }
    const vector<int>& getCalificaciones() const { return calificaciones; }

    // Método para serializar el objeto a sintaxis JSON
    string aJSON() const {
        string json = "    {\n";
        json += "        \"id\": " + to_string(id) + ",\n";
        json += "        \"nombre\": \"" + nombre + "\",\n";
        json += "        \"calificaciones\": [";
        for (size_t i = 0; i < calificaciones.size(); ++i) {
            json += to_string(calificaciones[i]);
            if (i < calificaciones.size() - 1) json += ", ";
        }
        json += "]\n";
        json += "    }";
        return json;
    }
};

// ==========================================
// 2. INTERFAZ DE PERSISTENCIA - OCP / DIP
// ==========================================
class IPersistenciaJSON {
public:
    virtual void escribir(const string& ruta, const vector<shared_ptr<ElementoADT>>& elementos) = 0;
    virtual vector<shared_ptr<ElementoADT>> leer(const string& ruta) = 0;
    virtual ~IPersistenciaJSON() = default;
};

// ==========================================
// 3. IMPLEMENTACIÓN CONCRETA - LSP / SRP
// ==========================================
class ManejadorJSON : public IPersistenciaJSON {
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
            if (i < elementos.size() - 1) {
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
        string nombre = "";
        vector<int> cals;
        bool leyendoCals = false;

        while (getline(archivo, linea)) {
            if (linea.find("\"id\":") != string::npos) {
                size_t dosPuntos = linea.find(":");
                id = stoi(linea.substr(dosPuntos + 1));
            }
            else if (linea.find("\"nombre\":") != string::npos) {
                size_t inicio = linea.find("\"") + 1; // buscar segunda comilla
                inicio = linea.find(":", inicio) + 2;
                size_t fin = linea.find_last_of("\"");
                nombre = linea.substr(inicio, fin - inicio);
            }
            else if (linea.find("\"calificaciones\":") != string::npos) {
                cals.clear();
                leyendoCals = true;
                // Si el arreglo viene en la misma línea o en varias
                if (linea.find("]") != string::npos) {
                    // Está en la misma línea
                    size_t inicio = linea.find("[") + 1;
                    size_t fin = linea.find("]");
                    string nums = linea.substr(inicio, fin - inicio);
                    stringstream ss(nums);
                    string num;
                    while (getline(ss, num, ',')) {
                        // limpiar espacios
                        num.erase(0, num.find_first_not_of(" \t"));
                        num.erase(num.find_last_not_of(" \t") + 1);
                        if (!num.empty()) cals.push_back(stoi(num));
                    }
                    leyendoCals = false;
                }
            }
            else if (leyendoCals) {
                // Si los números del arreglo vienen distribuidos o en una línea independiente
                size_t inicio = linea.find_first_not_of(" \t");
                if (inicio != string::npos) {
                    string sub = linea.substr(inicio);
                    if (sub.find("]") != string::npos) {
                        leyendoCals = false;
                    } else {
                        // Extraer números limpios
                        string limpio = "";
                        for (char c : sub) {
                            if (isdigit(c)) limpio += c;
                            else if (!limpio.empty() && (c == ',' || c == ' ')) {
                                cals.push_back(stoi(limpio));
                                limpio = "";
                            }
                        }
                        if (!limpio.empty()) cals.push_back(stoi(limpio));
                    }
                }
            }

            // Al cerrar la llave de un objeto JSON, creamos el ADT
            if (linea.find("}") != string::npos && id != 0) {
                elementos.push_back(make_shared<ElementoADT>(id, nombre, cals));
                id = 0;
                nombre = "";
                cals.clear();
            }
        }

        archivo.close();
        cout << "\n[EXITO] Datos leidos y recuperados desde el archivo JSON: " << ruta << "\n";
        return elementos;
    }
};

// ==========================================
// 4. CLASE GESTORA DEL SISTEMA (POO)
// ==========================================
class GestorSistema {
private:
    vector<shared_ptr<ElementoADT>> coleccionDatos;
    unique_ptr<IPersistenciaJSON> persistencia;

public:
    GestorSistema(unique_ptr<IPersistenciaJSON> pers) : persistencia(move(pers)) {}

    void agregarElemento(int id, string nombre, vector<int> cals) {
        coleccionDatos.push_back(make_shared<ElementoADT>(id, nombre, cals));
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
        cout << "\n=========================================\n";
        cout << "       REGISTROS ACTUALES EN MEMORIA     \n";
        cout << "=========================================\n";
        for (const auto& elem : coleccionDatos) {
            cout << "ID: " << elem->getId() 
                 << " | Nombre: " << elem->getNombre() 
                 << " | Arreglo de Valores: [ ";
            for (int c : elem->getCalificaciones()) {
                cout << c << " ";
            }
            cout << "]\n";
        }
        cout << "=========================================\n";
    }
};

// ==========================================
// 5. FUNCION PRINCIPAL CON MENU INTERACTIVO
// ==========================================
int main() {
    unique_ptr<IPersistenciaJSON> servicioJSON = make_unique<ManejadorJSON>();
    GestorSistema sistema(move(servicioJSON));

    int opcion;
    string nombreArchivo = "ejercicio_datos.json";

    do {
        cout << "\n=========================================\n";
        cout << "     MENU INTERACTIVO - ARCHIVOS JSON    \n";
        cout << "=========================================\n";
        cout << "1. Agregar nuevo elemento (ADT)\n";
        cout << "2. Ver elementos en memoria RAM\n";
        cout << "3. Guardar elementos en archivo JSON (Disco)\n";
        cout << "4. Cargar elementos desde archivo JSON (Disco)\n";
        cout << "5. Salir\n";
        cout << "Selecciona una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                int id;
                string nombre;
                int v1, v2, v3;

                cout << "Ingresa el ID numerico: ";
                cin >> id;
                cout << "Ingresa el Nombre o Descriptor: ";
                cin >> nombre;
                cout << "Ingresa 3 valores numericos para el arreglo (separados por espacio): ";
                cin >> v1 >> v2 >> v3;

                vector<int> arreglo = {v1, v2, v3};
                sistema.agregarElemento(id, nombre, arreglo);
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
                cout << "\nSaliendo del programa. ¡Hasta luego!\n";
                break;
            default:
                cout << "\n[ERROR] Opcion no valida. Intenta de nuevo.\n";
        }
    } while (opcion != 5);

    return 0;
}