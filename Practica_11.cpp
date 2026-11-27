#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>

using namespace std;

// ==========================================
// 1. DATO COMPLEJO (SRP / COMPOSICION)
// Representa un objeto compuesto individual
// ==========================================
class ConfiguracionCompleja {
private:
    string area;
    int prioridad;
    double factor;

public:
    ConfiguracionCompleja(string a = "General", int p = 1, double f = 1.0)
        : area(a), prioridad(p), factor(f) {}

    string getArea() const { return area; }
    int getPrioridad() const { return prioridad; }
    double getFactor() const { return factor; }

    // Usa '|' para separar atributos internos sin romper las comas del CSV
    string serializar() const {
        return area + "|" + to_string(prioridad) + "|" + to_string(factor);
    }

    static ConfiguracionCompleja deserializar(const string& texto) {
        stringstream ss(texto);
        string a, pStr, fStr;
        getline(ss, a, '|');
        getline(ss, pStr, '|');
        getline(ss, fStr, '|');
        return ConfiguracionCompleja(a, stoi(pStr), stod(fStr));
    }

    void mostrar() const {
        cout << "{Area: " << area << ", Prioridad: " << prioridad << ", Factor: " << factor << "}";
    }
};

// ==========================================
// 2. ELEMENTO PARA ARREGLO COMPLEJO (SRP)
// Cada instancia forma parte de un arreglo de objetos
// ==========================================
class ModuloComplejo {
private:
    string clave;
    int nivel;
    double peso;

public:
    ModuloComplejo(string c = "MOD", int n = 1, double p = 0.5)
        : clave(c), nivel(n), peso(p) {}

    string getClave() const { return clave; }
    int getNivel() const { return nivel; }
    double getPeso() const { return peso; }

    string serializar() const {
        return clave + "|" + to_string(nivel) + "|" + to_string(peso);
    }

    static ModuloComplejo deserializar(const string& texto) {
        stringstream ss(texto);
        string c, nStr, pStr;
        getline(ss, c, '|');
        getline(ss, nStr, '|');
        getline(ss, pStr, '|');
        return ModuloComplejo(c, stoi(nStr), stod(pStr));
    }

    void mostrar() const {
        cout << "[" << clave << " | Niv:" << nivel << " | Peso:" << peso << "]";
    }
};

// ==========================================
// 3. TIPO DE DATO ABSTRACTO (ADT) - SRP
// Integra datos basicos, arreglo simple, dato complejo y arreglo complejo
// ==========================================
class ElementoADT {
private:
    int id;
    string nombre;
    vector<int> calificaciones;
    ConfiguracionCompleja configuracion;
    vector<ModuloComplejo> modulos;

public:
    ElementoADT(int id,
                string nombre,
                vector<int> cals,
                ConfiguracionCompleja config,
                vector<ModuloComplejo> mods)
        : id(id),
          nombre(nombre),
          calificaciones(cals),
          configuracion(config),
          modulos(mods) {}

    int getId() const { return id; }
    string getNombre() const { return nombre; }
    const vector<int>& getCalificaciones() const { return calificaciones; }
    const ConfiguracionCompleja& getConfiguracion() const { return configuracion; }
    const vector<ModuloComplejo>& getModulos() const { return modulos; }

    string aCSV() const {
        string linea = to_string(id) + "," + nombre + ",";

        // Arreglo simple separado por ';'
        for (size_t i = 0; i < calificaciones.size(); ++i) {
            linea += to_string(calificaciones[i]);
            if (i + 1 < calificaciones.size()) linea += ";";
        }

        // Dato complejo en su propia columna CSV
        linea += "," + configuracion.serializar() + ",";

        // Arreglo complejo con elementos separados por ';'
        for (size_t i = 0; i < modulos.size(); ++i) {
            linea += modulos[i].serializar();
            if (i + 1 < modulos.size()) linea += ";";
        }

        return linea;
    }
};

// ==========================================
// 4. INTERFAZ DE PERSISTENCIA - OCP / DIP
// ==========================================
class IPersistenciaCSV {
public:
    virtual void escribir(const string& ruta, const vector<shared_ptr<ElementoADT>>& elementos) = 0;
    virtual vector<shared_ptr<ElementoADT>> leer(const string& ruta) = 0;
    virtual ~IPersistenciaCSV() = default;
};

// ==========================================
// 5. IMPLEMENTACION CONCRETA - LSP / SRP
// ==========================================
class ManejadorCSV : public IPersistenciaCSV {
public:
    void escribir(const string& ruta, const vector<shared_ptr<ElementoADT>>& elementos) override {
        ofstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "\n[ERROR] No se pudo abrir el archivo para escritura.\n";
            return;
        }

        archivo << "ID,Nombre,ArregloSimple,DatoComplejo,ArregloComplejo\n";
        for (const auto& elem : elementos) {
            archivo << elem->aCSV() << "\n";
        }

        archivo.close();
        cout << "\n[EXITO] Datos guardados correctamente en: " << ruta << "\n";
    }

    vector<shared_ptr<ElementoADT>> leer(const string& ruta) override {
        vector<shared_ptr<ElementoADT>> elementos;
        ifstream archivo(ruta);

        if (!archivo.is_open()) {
            cout << "\n[AVISO] No se encontro un archivo previo. Se iniciara vacio.\n";
            return elementos;
        }

        string linea;
        getline(archivo, linea); // Omitir cabecera

        while (getline(archivo, linea)) {
            if (linea.empty()) continue;

            stringstream ss(linea);
            string idStr, nombre, bloqueSimple, bloqueComplejo, bloqueArregloComplejo;

            getline(ss, idStr, ',');
            getline(ss, nombre, ',');
            getline(ss, bloqueSimple, ',');
            getline(ss, bloqueComplejo, ',');
            getline(ss, bloqueArregloComplejo, ',');

            // Reconstruir arreglo simple
            vector<int> cals;
            stringstream ssSimple(bloqueSimple);
            string numStr;
            while (getline(ssSimple, numStr, ';')) {
                if (!numStr.empty()) cals.push_back(stoi(numStr));
            }

            // Reconstruir dato complejo
            ConfiguracionCompleja config = ConfiguracionCompleja::deserializar(bloqueComplejo);

            // Reconstruir arreglo complejo
            vector<ModuloComplejo> mods;
            stringstream ssMods(bloqueArregloComplejo);
            string itemMod;
            while (getline(ssMods, itemMod, ';')) {
                if (!itemMod.empty()) {
                    mods.push_back(ModuloComplejo::deserializar(itemMod));
                }
            }

            elementos.push_back(make_shared<ElementoADT>(stoi(idStr), nombre, cals, config, mods));
        }

        archivo.close();
        cout << "\n[EXITO] Datos leidos y reconstruidos desde: " << ruta << "\n";
        return elementos;
    }
};

// ==========================================
// 6. CLASE GESTORA DEL SISTEMA (POO)
// ==========================================
class GestorSistema {
private:
    vector<shared_ptr<ElementoADT>> coleccionDatos;
    unique_ptr<IPersistenciaCSV> persistencia;

public:
    GestorSistema(unique_ptr<IPersistenciaCSV> pers) : persistencia(move(pers)) {}

    void agregarElemento(int id,
                         string nombre,
                         vector<int> cals,
                         ConfiguracionCompleja config,
                         vector<ModuloComplejo> mods) {
        coleccionDatos.push_back(make_shared<ElementoADT>(id, nombre, cals, config, mods));
        cout << "\n[INFO] Elemento completo agregado a la memoria RAM.\n";
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
                 << " | Arreglo Simple: [ ";
            for (int c : elem->getCalificaciones()) {
                cout << c << " ";
            }
            cout << "]\n   -> Dato Complejo: ";
            elem->getConfiguracion().mostrar();
            cout << "\n   -> Arreglo Complejo: [ ";
            for (size_t i = 0; i < elem->getModulos().size(); ++i) {
                elem->getModulos()[i].mostrar();
                if (i + 1 < elem->getModulos().size()) cout << ", ";
            }
            cout << " ]\n";
        }
        cout << "=======================================================================\n";
    }
};

// ==========================================
// 7. FUNCION PRINCIPAL CON MENU INTERACTIVO
// ==========================================
int main() {
    unique_ptr<IPersistenciaCSV> servicioCSV = make_unique<ManejadorCSV>();
    GestorSistema sistema(move(servicioCSV));

    int opcion;
    string nombreArchivo = "ejercicio11_completo.csv";

    do {
        cout << "\n=========================================================\n";
        cout << "   MENU - CSV  \n";
        cout << "=========================================================\n";
        cout << "1. Agregar elemento completo\n";
        cout << "2. Ver elementos en memoria RAM\n";
        cout << "3. Guardar en archivo CSV (Disco)\n";
        cout << "4. Cargar desde archivo CSV (Disco)\n";
        cout << "5. Salir\n";
        cout << "Selecciona una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                int id, cantSimples, cantComplejos;
                string nombre;

                cout << "Ingresa el ID numerico: ";
                cin >> id;
                cout << "Ingresa el Nombre o Descriptor: ";
                cin >> nombre;

                // 1. Arreglo simple
                cout << "Cuantos valores tendra el arreglo simple?: ";
                cin >> cantSimples;
                vector<int> arregloSimple(cantSimples);
                for (int i = 0; i < cantSimples; ++i) {
                    cout << "  Valor simple [" << i + 1 << "]: ";
                    cin >> arregloSimple[i];
                }

                // 2. Dato complejo individual
                string area;
                int prioridad;
                double factor;
                cout << "Dato complejo - Area (sin espacios): ";
                cin >> area;
                cout << "Dato complejo - Prioridad (entero): ";
                cin >> prioridad;
                cout << "Dato complejo - Factor (decimal): ";
                cin >> factor;
                ConfiguracionCompleja config(area, prioridad, factor);

                // 3. Arreglo complejo
                cout << "Cuantos objetos tendra el arreglo complejo?: ";
                cin >> cantComplejos;
                vector<ModuloComplejo> arregloComplejo;
                for (int i = 0; i < cantComplejos; ++i) {
                    string clave;
                    int nivel;
                    double peso;
                    cout << "  Modulo [" << i + 1 << "] - Clave (sin espacios): ";
                    cin >> clave;
                    cout << "  Modulo [" << i + 1 << "] - Nivel (entero): ";
                    cin >> nivel;
                    cout << "  Modulo [" << i + 1 << "] - Peso (decimal): ";
                    cin >> peso;
                    arregloComplejo.emplace_back(clave, nivel, peso);
                }

                sistema.agregarElemento(id, nombre, arregloSimple, config, arregloComplejo);
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