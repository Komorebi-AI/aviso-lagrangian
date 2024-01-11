Lagrangian analysis
===================

Introduction
------------

Finite Size Lyapunov Exponent (FSLE) is a local lagrangian diagnostics that is
widely used for the study of transport and mixing processes of oceanographic
tracers (Sea surface temperature, Ocean color ...). Its computation is derived
from the definition of Finite-Time Lyapunov Exponent that allows the
identification of  Lagrangian Coherent Structures.

The code used to compute LE, was developed in collaboration between LOCEAN (F.
d'Ovidio) and CLS, is available under GNU General Public License

Given a time series of meso-scale velocity field, this code computes
corresponding sub-mesoscale maps of backward or forward FLSE or FTLE. It also
compute other diagnostics such as orientation of Finite-Time Lyapunov Vectors.

License
-------

Lagrangian is distributed under the GNU Lesser General Public License. See
COPYING for details.

Documentation
-------------

For full documentation visit the [Documentation
Page](http://lagrangian.readthedocs.io/en/latest/index.html).

Instalar a partir de código fuente
----------------------------------

1. Crear entorno con dependencias:

   ```{bash}
   conda install -c conda-forge boost udunits2 netcdf4
   ```

2. Obtener submodulos:

   ```{bash}
   git submodule update --init --recursive
   ```

3. Instalar:

   ```{bash}
   pip install .
   ```

   ```{bash}
   export UDUNITS2_XML_PATH=/home/atorres/miniconda3/envs/RTD/share/udunits/udunits2.xml
   ```

4. Ejecutar:

   ```{bash}
   time -v map_of_fle sds.ini 2023_07_18.nc "2023-07-18" --advection_time 10 --resolution=0.04 --final_separation 0.2 --x_min -180 --x_max 180 --y_min -60 --y_max 60 --verbose --time_direction forward
   ```

Instalar versión de Conda
-------------------------

1. Instalar entorno del proyecto y activar:

   ```{bash}
   conda env create -f env.yml
   conda activate aviso-lagrangian
   ```

2. Ver localización paquete

   ```{bash}
   python -c "import lagrangian; print(lagrangian.__file__)"
   ```

3. Entrar en el directorio (quitar `__init__.py` del path anterior) y a continuación comentar parte de la línea 545 del fichero `console_scripts/map_of_fle.py`:

   ```{python}
   if HAVE_DASK and args.scheduler_file is not None: #or args.local_cluster:
   ```

4. Copiar los ficheros necesarios usando el script `convert_nc.py` (usar cualquier entorno de conda con `xarray` y `pandas`):

   ```{bash}
   python convert_nc.py /home/atorres/data/uv OUTPUT_DIR
   ```

5. Editar el fichero `sds.ini` y escribir una línea por cada una de las fechas necesarias

6. Ejecutar:

   ```{bash}
   map_of_fle sds.ini 2023_08_07.nc "2023-08-07" --advection_time 20 --resolution=0.04 --final_separation 0.6 --initial_separation 0.2 --x_min -179.98 --x_max 179.98 --y_min -59.98 --y_max 59.98 --verbose --time_direction backward
   ```

Docker
------

1. Construir imágen

   ```{bash}
   docker build -t lagrangian .
   ```

2. Ejecutar contenedor

   ```{bash}
   docker run --user=root -it -v /home/SHARED/SATLINK/fsle:/home/mambauser/data lagrangian bash
   ```

3. Correr script del cálculo del FSLE

   ```{bash}
   python map_of_fle.py ./data/uv ./data/fsle "2023-02-28 12:00" --advection_time 20 --final_separation 0.2 --x_min -180 --nx 4320 --y_min -60 --ny 1441 --resolution 1/12 --time_direction backward
   ```

Descargar datos de FSLE
-----------------------

1. Instalar motuclient con

   ```{bash}
   pip install motuclient
   ```

2. Ejecutar cambiando la fecha (`-t` y `-T`) y el fichero de salida (`-f`):

   ```{bash}
   motuclient -u albertotb@gmail.com -p 2BFeqk -m https://motu.aviso.altimetry.fr/motu-web/Motu -s AvisoFSLE -d dataset-duacs-nrt-global-allsat-madt-fsle -x -180 -X 180 -y -60 -Y 60 -t "2022-07-15" -T "2022-07-15" --outputWritten netcdf -v fsle_max -v theta_max -o "." -f "2022_07_15_fsle.nc"
   ```

3. Otro ejemplo:

   ```{bash}
   motuclient -u albertotb@gmail.com -p 2BFeqk -m https://motu.aviso.altimetry.fr/motu-web/Motu -s AvisoFSLE -d dataset-duacs-nrt-global-allsat-madt-fsle -x -180 -X 180 -y -90 -Y 90 -t "2022-08-07" -T "2022-08-07" --outputWritten netcdf -v fsle_max -v theta_max -o "." -f "fsle_2022_08_07.nc"
   ```

Otra forma alternativa, desde la web:

1. Entrar en [https://www.aviso.altimetry.fr/en/home.html](https://www.aviso.altimetry.fr/en/home.html)
2. Identificarse con usuario (`albertotb@gmail.com`) y contraseña (`2BFeqk`)
3. Ir a `MY AVISO+ > My products > Lyapunov Exponents and Orientations (grids) > Extraction > DATASET-DUACS-NRT-GLOBAL-ALLSAT-MADT-FSLE`. Seleccionar `Full region` y editar la latitud a (-60, 60). Editar la fecha en el desplegable (importante destacar que la fecha que aparece es la última disponible)

NOTA: Los ficheros descargados de AVISO tienen 9000 longitudes y 3000 latitudes. Para conseguir algo similar con el script, habría que usar `--resolution 0.04`. Sin embargo, no es idéntico.
Además, los ficheros descargados tienen las siguientes longitudes:

```{text}
* lon       (lon) float64 180.0 180.0 180.1 180.1 ... 539.8 539.9 539.9 539.9
```

Esto no se corresponde con el formato habitual (-180, 180), habría que ver como transformarlo

Pruebas AVISO
-------------

```{bash}
map_of_fle duacs.ini 2023_08_07.nc "2023-08-07" --advection_time 20 --resolution=0.04 --final_separation 0.6 --initial_separation 0.2 --x_min -179.98 --x_max 179.98 --y_min -59.98 --y_max 59.98 --verbose --time_direction backward
```
