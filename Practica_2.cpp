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

    // Método para serializar el objeto a formato de texto plano legible
    string aTXT() const {
        string linea = to_string(id) + " | " + nombre + " | ";
        for (int cal : calificaciones) {
            linea += to_string(cal) + " ";
        }
        return linea;
    }
};

// ==========================================
// 2. INTERFAZ DE PERSISTENCIA - OCP / DIP
// ==========================================
class IPersistenciaTXT {
public:
    virtual void escribir(const string& ruta, const vector<shared_ptr<ElementoADT>>& elementos) = 0;
    virtual vector<shared_ptr<ElementoADT>> leer(const string& ruta) = 0;
    virtual ~IPersistenciaTXT() = default;
};

// ==========================================
// 3. IMPLEMENTACIÓN CONCRETA - LSP / SRP
// ==========================================
class ManejadorTXT : public IPersistenciaTXT {
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
        // Omitir la cabecera del reporte
        getline(archivo, linea);

        while (getline(archivo, linea)) {
            if (linea.empty() || linea.find("---") != string::npos) continue;

            stringstream ss(linea);
            string dato;
            
            // Leer ID
            getline(ss, dato, '|');
            // Limpiar espacios en blanco
            dato.erase(0, dato.find_first_not_of(" \t"));
            dato.erase(dato.find_last_not_of(" \t") + 1);
            int id = stoi(dato);

            // Leer Nombre
            getline(ss, dato, '|');
            dato.erase(0, dato.find_first_not_of(" \t"));
            dato.erase(dato.find_last_not_of(" \t") + 1);
            string nombre = dato;

            // Leer Arreglo de valores restantes
            getline(ss, dato);
            stringstream ssCals(dato);
            vector<int> cals;
            int val;
            while (ssCals >> val) {
                cals.push_back(val);
            }

            elementos.push_back(make_shared<ElementoADT>(id, nombre, cals));
        }

        archivo.close();
        cout << "\n[EXITO] Datos leidos y recuperados desde el archivo TXT: " << ruta << "\n";
        return elementos;
    }
};

// ==========================================
// 4. CLASE GESTORA DEL SISTEMA (POO)
// ==========================================
class GestorSistema {
private:
    vector<shared_ptr<ElementoADT>> coleccionDatos;
    unique_ptr<IPersistenciaTXT> persistencia;

public:
    GestorSistema(unique_ptr<IPersistenciaTXT> pers) : persistencia(move(pers)) {}

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
    unique_ptr<IPersistenciaTXT> servicioTXT = make_unique<ManejadorTXT>();
    GestorSistema sistema(move(servicioTXT));

    int opcion;
    string nombreArchivo = "ejercicio_datos.txt";

    do {
        cout << "\n=========================================\n";
        cout << "     MENU - ARCHIVOS TXT     \n";
        cout << "=========================================\n";
        cout << "1. Agregar nuevo elemento (ADT)\n";
        cout << "2. Ver elementos en memoria RAM\n";
        cout << "3. Guardar elementos en archivo TXT (Disco)\n";
        cout << "4. Cargar elementos desde archivo TXT (Disco)\n";
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