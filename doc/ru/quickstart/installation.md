# Руководство по установке

В этом руководстве описано, как установить Python-Cellframe SDK.

## 1. Установка из DEB пакетов (Рекомендуется)

Самый простой способ — использовать готовые DEB пакеты.

### Требования
*   Debian 11/12 или Ubuntu 20.04/22.04
*   Python 3.9+

### Шаги установки

1.  Скачайте последние версии пакетов (`dap-sdk`, `cellframe-sdk`, `python-dap`, `python-cellframe`).
2.  Установите их в следующем порядке:

```bash
sudo dpkg -i dap-sdk_*.deb
sudo dpkg -i cellframe-sdk_*.deb
sudo dpkg -i python-dap_*.deb
sudo dpkg -i python-cellframe_*.deb
```

3.  Исправьте зависимости, если потребуется:

```bash
sudo apt-get install -f
```

## 2. Сборка из исходников

Если вам нужна последняя версия для разработки, вы можете собрать проект из исходного кода.

### Требования
*   CMake 3.10+
*   GCC/Clang
*   Заголовочные файлы Python 3 (`python3-dev`)

### Сборка

```bash
git clone https://gitlab.demlabs.net/cellframe/python-cellframe.git
cd python-cellframe
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

## 3. Проверка установки

Проверьте, импортируются ли модули в Python:

```bash
python3 -c "import CellFrame; print(f'Версия CellFrame: {CellFrame.get_version()}')"
```

