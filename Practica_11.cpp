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

    string aCSV() const {
        string linea = to_string(id) + "," + nombre;
        for (int cal : calificaciones) {
            linea += "," + to_string(cal);
        }
        return linea;
    }
};

// ==========================================
// 2. INTERFAZ DE PERSISTENCIA - OCP / DIP
// ==========================================
class IPersistenciaCSV {
public:
    virtual void escribir(const string& ruta, const vector<shared_ptr<ElementoADT>>& elementos) = 0;
    virtual vector<shared_ptr<ElementoADT>> leer(const string& ruta) = 0;
    virtual ~IPersistenciaCSV() = default;
};

// ==========================================
// 3. IMPLEMENTACIÓN CONCRETA - LSP / SRP
// ==========================================
class ManejadorCSV : public IPersistenciaCSV {
public:
    void escribir(const string& ruta, const vector<shared_ptr<ElementoADT>>& elementos) override {
        ofstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "\n[ERROR] No se pudo abrir el archivo para escritura.\n";
            return;
        }

        archivo << "ID,Nombre,Valor1,Valor2,Valor3\n";
        for (const auto& elem : elementos) {
            archivo << elem->aCSV() << "\n";
        }

        archivo.close();
        cout << "\n[EXITO] Datos guardados correctamente en el archivo: " << ruta << "\n";
    }

    vector<shared_ptr<ElementoADT>> leer(const string& ruta) override {
        vector<shared_ptr<ElementoADT>> elementos;
        ifstream archivo(ruta);
        
        if (!archivo.is_open()) {
            cout << "\n[AVISO] No se encontro un archivo previo en la ruta. Se iniciara vacio.\n";
            return elementos;
        }

        string linea;
        getline(archivo, linea); // Omitir cabecera

        while (getline(archivo, linea)) {
            stringstream ss(linea);
            string dato;
            
            getline(ss, dato, ',');
            int id = stoi(dato);

            getline(ss, dato, ',');
            string nombre = dato;

            vector<int> cals;
            while (getline(ss, dato, ',')) {
                cals.push_back(stoi(dato));
            }

            elementos.push_back(make_shared<ElementoADT>(id, nombre, cals));
        }

        archivo.close();
        cout << "\n[EXITO] Datos leidos y recuperados desde el archivo: " << ruta << "\n";
        return elementos;
    }
};

// ==========================================
// 4. CLASE GESTORA DEL SISTEMA (POO)
// ==========================================
class GestorSistema {
private:
    vector<shared_ptr<ElementoADT>> coleccionDatos;
    unique_ptr<IPersistenciaCSV> persistencia;

public:
    GestorSistema(unique_ptr<IPersistenciaCSV> pers) : persistencia(move(pers)) {}

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
    // Inyeccion de dependencias (DIP)
    unique_ptr<IPersistenciaCSV> servicioCSV = make_unique<ManejadorCSV>();
    GestorSistema sistema(move(servicioCSV));

    int opcion;
    string nombreArchivo = "ejercicio11_datos.csv";

    do {
        cout << "\n=========================================\n";
        cout << "     MENU  - ARCHIVOS CSV     \n";
        cout << "=========================================\n";
        cout << "1. Agregar nuevo elemento (ADT)\n";
        cout << "2. Ver elementos en memoria RAM\n";
        cout << "3. Guardar elementos en archivo CSV (Disco)\n";
        cout << "4. Cargar elementos desde archivo CSV (Disco)\n";
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