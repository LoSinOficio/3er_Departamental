#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <limits>

using namespace std;

// ==========================================
// UTILIDAD DE TEXTO Y PARSEO SEGURO (SRP)
// ==========================================
class UtilTXT {
public:
    static string limpiar(const string& s) {
        size_t ini = s.find_first_not_of(" \t\r\n");
        if (ini == string::npos) return "";
        size_t fin = s.find_last_not_of(" \t\r\n");
        return s.substr(ini, fin - ini + 1);
    }

    static string escapar(const string& texto) {
        string out;
        out.reserve(texto.size());
        for (char c : texto) {
            switch (c) {
                case '\\': out += "\\\\"; break;
                case '|':  out += "\\p"; break;
                case ',':  out += "\\c"; break;
                case ';':  out += "\\s"; break;
                case ':':  out += "\\d"; break;
                default:   out += c; break;
            }
        }
        return out;
    }

    static string desescapar(const string& texto) {
        string out;
        out.reserve(texto.size());
        for (size_t i = 0; i < texto.size(); ++i) {
            if (texto[i] == '\\' && i + 1 < texto.size()) {
                char sig = texto[i + 1];
                if (sig == '\\') { out += '\\'; ++i; }
                else if (sig == 'p') { out += '|'; ++i; }
                else if (sig == 'c') { out += ','; ++i; }
                else if (sig == 's') { out += ';'; ++i; }
                else if (sig == 'd') { out += ':'; ++i; }
                else out += texto[i];
            } else {
                out += texto[i];
            }
        }
        return out;
    }

    static vector<string> separarEscapado(const string& texto, char sep) {
        vector<string> partes;
        string actual;
        for (size_t i = 0; i < texto.size(); ++i) {
            if (texto[i] == '\\' && i + 1 < texto.size()) {
                actual += texto[i];
                actual += texto[i + 1];
                ++i;
            } else if (texto[i] == sep) {
                partes.push_back(limpiar(actual));
                actual.clear();
            } else {
                actual += texto[i];
            }
        }
        partes.push_back(limpiar(actual));
        return partes;
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
// 1. LOTE / PRESENTACION COMPLEJA DE FRUTA (SRP)
// ==========================================
class ItemComplejo {
public:
    string clave;
    string descripcion;
    int nivel;
    double peso;

    ItemComplejo(string c = "FRU-GEN",
                 string desc = "Sin descripcion",
                 int n = 1,
                 double p = 1.0)
        : clave(c), descripcion(desc), nivel(n), peso(p) {}

    string aTXT() const {
        return UtilTXT::escapar(clave) + ":" +
               UtilTXT::escapar(descripcion) + ":" +
               to_string(nivel) + ":" +
               to_string(peso);
    }

    static ItemComplejo desdeTXT(const string& texto) {
        vector<string> partes = UtilTXT::separarEscapado(texto, ':');
        string c = partes.size() > 0 && !partes[0].empty() ? UtilTXT::desescapar(partes[0]) : "FRU-GEN";
        string d = partes.size() > 1 && !partes[1].empty() ? UtilTXT::desescapar(partes[1]) : "Sin descripcion";
        int niv = 1;
        double pes = 1.0;
        if (partes.size() > 2) UtilTXT::parsearEnteroSeguro(partes[2], niv);
        if (partes.size() > 3) UtilTXT::parsearDoubleSeguro(partes[3], pes);
        return ItemComplejo(c, d, niv, pes);
    }

    void mostrar() const {
        cout << "[" << clave << " | \"" << descripcion
             << "\" | Madurez/Cat:" << nivel << " | Peso(kg):" << peso << "]";
    }
};

// ==========================================
// 2. TIPO DE DATO ABSTRACTO DE FRUTERIA (ADT) - SRP
// ==========================================
class ElementoADT {
private:
    int id;
    string nombre;
    string descripcion;
    vector<string> simples;
    ItemComplejo complejo;
    vector<ItemComplejo> arreglosComplejos;

public:
    ElementoADT(int id,
                string nombre,
                string descripcion,
                vector<string> s,
                ItemComplejo c,
                vector<ItemComplejo> ac)
        : id(id),
          nombre(nombre),
          descripcion(descripcion),
          simples(s),
          complejo(c),
          arreglosComplejos(ac) {}

    int getId() const { return id; }
    string getNombre() const { return nombre; }
    string getDescripcion() const { return descripcion; }
    const vector<string>& getSimples() const { return simples; }
    const ItemComplejo& getComplejo() const { return complejo; }
    const vector<ItemComplejo>& getArreglosComplejos() const { return arreglosComplejos; }

    string aTXT() const {
        string linea = to_string(id) + " | " +
                       UtilTXT::escapar(nombre) + " | " +
                       UtilTXT::escapar(descripcion) + " | ";

        for (size_t i = 0; i < simples.size(); ++i) {
            linea += UtilTXT::escapar(simples[i]);
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
public:
    void escribir(const string& ruta, const vector<shared_ptr<ElementoADT>>& elementos) override {
        ofstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "\n[ERROR] No se pudo abrir el archivo TXT de frutería para escritura.\n";
            return;
        }

        archivo << "--- REPORTE DE INVENTARIO DE FRUTERIA (TXT) ---\n";
        for (const auto& elem : elementos) {
            archivo << elem->aTXT() << "\n";
        }

        archivo.close();
        cout << "\n[EXITO] Datos de frutería guardados correctamente en: " << ruta << "\n";
    }

    vector<shared_ptr<ElementoADT>> leer(const string& ruta) override {
        vector<shared_ptr<ElementoADT>> elementos;
        ifstream archivo(ruta);

        if (!archivo.is_open()) {
            cout << "\n[AVISO] No se encontro un archivo TXT previo de frutería. Se iniciara vacio.\n";
            return elementos;
        }

        string linea;
        while (getline(archivo, linea)) {
            string limp = UtilTXT::limpiar(linea);
            if (limp.empty() || limp.find("---") != string::npos) continue;

            vector<string> campos = UtilTXT::separarEscapado(limp, '|');
            if (campos.size() < 5) continue;

            int id = 0;
            if (!UtilTXT::parsearEnteroSeguro(campos[0], id)) continue;

            string nombre = UtilTXT::desescapar(campos[1]);
            string descripcion = UtilTXT::desescapar(campos[2]);

            vector<string> simples;
            if (!campos[3].empty()) {
                for (const auto& valStr : UtilTXT::separarEscapado(campos[3], ',')) {
                    if (!valStr.empty()) {
                        simples.push_back(UtilTXT::desescapar(valStr));
                    }
                }
            }

            ItemComplejo complejo = ItemComplejo::desdeTXT(campos[4]);

            vector<ItemComplejo> arreglosComplejos;
            if (campos.size() >= 6 && !campos[5].empty()) {
                for (const auto& itemStr : UtilTXT::separarEscapado(campos[5], ';')) {
                    if (!itemStr.empty()) {
                        arreglosComplejos.push_back(ItemComplejo::desdeTXT(itemStr));
                    }
                }
            }

            elementos.push_back(
                make_shared<ElementoADT>(id, nombre, descripcion, simples, complejo, arreglosComplejos)
            );
        }

        archivo.close();
        cout << "\n[EXITO] Datos de frutería leidos y recuperados desde: " << ruta << "\n";
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

    bool estaVacia() const {
        return coleccionDatos.empty();
    }

    size_t cantidadElementos() const {
        return coleccionDatos.size();
    }

    void agregarElemento(int id,
                         string nombre,
                         string descripcion,
                         vector<string> simples,
                         ItemComplejo complejo,
                         vector<ItemComplejo> arreglosComplejos) {
        coleccionDatos.push_back(
            make_shared<ElementoADT>(id, nombre, descripcion, simples, complejo, arreglosComplejos)
        );
        cout << "\n[INFO] Fruta/producto agregado exitosamente a la memoria RAM.\n";
    }

    bool editarElemento(size_t indice,
                        int id,
                        string nombre,
                        string descripcion,
                        vector<string> simples,
                        ItemComplejo complejo,
                        vector<ItemComplejo> arreglosComplejos) {
        if (indice >= coleccionDatos.size()) return false;
        coleccionDatos[indice] = make_shared<ElementoADT>(
            id, nombre, descripcion, simples, complejo, arreglosComplejos
        );
        cout << "\n[INFO] Registro [" << (indice + 1) << "] actualizado en la memoria RAM.\n";
        return true;
    }

    bool eliminarElemento(size_t indice) {
        if (indice >= coleccionDatos.size()) return false;
        coleccionDatos.erase(coleccionDatos.begin() + static_cast<long>(indice));
        cout << "\n[INFO] Registro [" << (indice + 1) << "] eliminado de la memoria RAM.\n";
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
            cout << "\n[AVISO] No hay frutas o productos registrados en la memoria actualmente.\n";
            return;
        }
        cout << "\n--- INVENTARIO DE FRUTERIA EN MEMORIA RAM ---\n";
        for (size_t i = 0; i < coleccionDatos.size(); ++i) {
            cout << (i + 1) << ". ID: " << coleccionDatos[i]->getId()
                 << " | Fruta/Producto: " << coleccionDatos[i]->getNombre()
                 << " | Desc: " << coleccionDatos[i]->getDescripcion() << "\n";
        }
    }

    void mostrarEnConsola() const {
        if (coleccionDatos.empty()) {
            cout << "\n[AVISO] No hay frutas o productos registrados en la memoria actualmente.\n";
            return;
        }
        cout << "\n=======================================================================\n";
        cout << "                 REGISTROS DE FRUTERIA ACTUALES EN MEMORIA            \n";
        cout << "=======================================================================\n";
        for (size_t idx = 0; idx < coleccionDatos.size(); ++idx) {
            const auto& elem = coleccionDatos[idx];
            cout << "[" << (idx + 1) << "] ID: " << elem->getId()
                 << " | Fruta/Producto: " << elem->getNombre()
                 << "\n  Descripcion: " << elem->getDescripcion()
                 << "\n  Variedades/Etiquetas simples: [ ";
            for (size_t i = 0; i < elem->getSimples().size(); ++i) {
                cout << "\"" << elem->getSimples()[i] << "\"";
                if (i + 1 < elem->getSimples().size()) cout << ", ";
            }
            cout << " ]\n  Lote/Presentacion principal: ";
            elem->getComplejo().mostrar();
            cout << "\n  Otros lotes/presentaciones: [ ";
            for (size_t i = 0; i < elem->getArreglosComplejos().size(); ++i) {
                elem->getArreglosComplejos()[i].mostrar();
                if (i + 1 < elem->getArreglosComplejos().size()) cout << ", ";
            }
            cout << " ]\n-----------------------------------------------------------------------\n";
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

    string leerLinea(const string& prompt) {
        string linea;
        cout << prompt;
        getline(cin >> ws, linea);
        return linea;
    }

    void capturarDatosElemento(int& id,
                               string& nombre,
                               string& descripcion,
                               vector<string>& simples,
                               ItemComplejo& complejo,
                               vector<ItemComplejo>& arreglosComplejos) {
        id = leerEntero("Ingresa el ID numerico de la fruta/producto: ");
        nombre = leerLinea("Ingresa el Nombre de la fruta o producto (ej. Mango Ataulfo): ");
        descripcion = leerLinea("Ingresa una Descripcion general (ej. Dulce, de temporada y jugoso): ");

        int cantSimples = leerEntero("Cuantas variedades, colores o etiquetas simples tendra?: ");
        if (cantSimples < 0) cantSimples = 0;
        simples.assign(cantSimples, "");
        for (int i = 0; i < cantSimples; ++i) {
            simples[i] = leerLinea("  Variedad/etiqueta [" + to_string(i + 1) + "]: ");
        }

        cout << "\n--- Lote o presentacion principal (dato complejo) ---\n";
        string cClave = leerLinea("  Clave de lote (ej. LOTE-MNG-01): ");
        string cDesc = leerLinea("  Descripcion del lote (ej. Caja de exportacion o huacal): ");
        int cNivel = leerEntero("  Nivel de madurez o categoria (entero, ej. 1-5): ");
        double cPeso = leerDouble("  Peso en kg (decimal, ej. 12.5): ");
        complejo = ItemComplejo(cClave, cDesc, cNivel, cPeso);

        int cantComplejos = leerEntero("\nCuantos lotes o presentaciones adicionales tendra?: ");
        if (cantComplejos < 0) cantComplejos = 0;
        arreglosComplejos.clear();
        for (int i = 0; i < cantComplejos; ++i) {
            cout << "  -- Presentacion/lote [" << i + 1 << "] --\n";
            string k = leerLinea("    Clave: ");
            string d = leerLinea("    Descripcion: ");
            int nv = leerEntero("    Nivel de madurez o categoria (entero): ");
            double ps = leerDouble("    Peso en kg (decimal): ");
            arreglosComplejos.emplace_back(k, d, nv, ps);
        }
    }
}

// ==========================================
// 6. FUNCION PRINCIPAL CON MENU INTERACTIVO
// ==========================================
int main() {
    unique_ptr<IPersistenciaTXT> servicioTXT = make_unique<ManejadorTXT>();
    GestorSistema sistema(move(servicioTXT));

    int opcion;
    string nombreArchivo = "fruteria_adt.txt";

    do {
        cout << "\n=========================================================\n";
        cout << "        MENU - INVENTARIO DE FRUTERIA (ARCHIVOS TXT)     \n";
        cout << "=========================================================\n";
        cout << "1. Agregar nueva fruta o producto (ADT)\n";
        cout << "2. Ver inventario en memoria RAM\n";
        cout << "3. Guardar inventario en archivo TXT (Disco)\n";
        cout << "4. Cargar inventario desde archivo TXT (Disco)\n";
        cout << "5. Editar fruta o producto existente en memoria RAM\n";
        cout << "6. Eliminar fruta o producto existente en memoria RAM\n";
        cout << "7. Salir\n";
        opcion = leerEntero("Selecciona una opcion: ");

        switch (opcion) {
            case 1: {
                int id;
                string nombre, descripcion;
                vector<string> simples;
                ItemComplejo complejo;
                vector<ItemComplejo> arreglosComplejos;
                capturarDatosElemento(id, nombre, descripcion, simples, complejo, arreglosComplejos);
                sistema.agregarElemento(id, nombre, descripcion, simples, complejo, arreglosComplejos);
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
                    cout << "\n[AVISO] No hay registros en memoria RAM para editar.\n";
                    break;
                }
                sistema.listarResumido();
                int num = leerEntero("Selecciona el numero de registro a editar: ");
                if (num < 1 || static_cast<size_t>(num) > sistema.cantidadElementos()) {
                    cout << "\n[ERROR] Numero de registro fuera de rango.\n";
                    break;
                }
                int id;
                string nombre, descripcion;
                vector<string> simples;
                ItemComplejo complejo;
                vector<ItemComplejo> arreglosComplejos;
                capturarDatosElemento(id, nombre, descripcion, simples, complejo, arreglosComplejos);
                sistema.editarElemento(
                    static_cast<size_t>(num - 1), id, nombre, descripcion, simples, complejo, arreglosComplejos
                );
                break;
            }
            case 6: {
                if (sistema.estaVacia()) {
                    cout << "\n[AVISO] No hay registros en memoria RAM para eliminar.\n";
                    break;
                }
                sistema.listarResumido();
                int num = leerEntero("Selecciona el numero de registro a eliminar: ");
                if (num < 1 || static_cast<size_t>(num) > sistema.cantidadElementos()) {
                    cout << "\n[ERROR] Numero de registro fuera de rango.\n";
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