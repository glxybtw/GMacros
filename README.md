# GMacros

**EN** | [RU](#ru)

---

A lightweight Fortnite macro tool built with Qt6/C++. Features a clean dark UI with 4 configurable macros, system tray support, and hotkey polling.

## Features

- **Drag Macro** — Hold toggle bind to auto-drag edits with LMB
- **Double Edit** — Instantly double-edit on keypress
- **Pickaxe/Shotgun** — Arms on LMB press, fires slot key on release
- **Auto Build** — Binds wall, floor, stair, cone and place keys for rapid building

## Requirements

- Windows 10/11 (64-bit)
- No installation required

## Usage

1. Run `GMacros.exe` as usual (no admin required)
2. Select a macro from the dashboard
3. Configure your binds and settings
4. Click **Apply**
5. The app minimizes to system tray — right-click tray icon to restore or quit

## Build from Source

**Requirements:**
- Qt 6.x (MinGW 64-bit)
- CMake 3.16+

```bash
# 1. Compile icon resource
windres -i app.rc -o app_icon.o --output-format=coff

# 2. Build
cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/mingw_64"
cmake --build build --config Release

# 3. Deploy Qt dependencies
windeployqt --no-translations build/GMacros.exe
```

## License

This project is licensed under the **GNU General Public License v3.0** — see [LICENSE](LICENSE) for details.

---

<a name="ru"></a>
# GMacros (RU)

Лёгкий инструмент для макросов в Fortnite, написанный на Qt6/C++. Тёмный UI, 4 настраиваемых макроса, поддержка системного трея и опрос горячих клавиш.

## Возможности

- **Drag Macro** — Удерживай бинд для автоматического drag-редактирования с ЛКМ
- **Double Edit** — Мгновенный двойной edit по нажатию клавиши
- **Pickaxe/Shotgun** — Взводится при нажатии ЛКМ, срабатывает при отпускании
- **Auto Build** — Бинды на стену, пол, лестницу, конус и размещение для быстрого строительства

## Требования

- Windows 10/11 (64-bit)
- Установка не требуется

## Использование

1. Запусти `GMacros.exe` (права администратора не нужны)
2. Выбери макрос на дашборде
3. Настрой бинды и параметры
4. Нажми **Apply**
5. Приложение сворачивается в трей — правой кнопкой по иконке для восстановления или выхода

## Сборка из исходников

**Требования:**
- Qt 6.x (MinGW 64-bit)
- CMake 3.16+

```bash
# 1. Компиляция иконки
windres -i app.rc -o app_icon.o --output-format=coff

# 2. Сборка
cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/mingw_64"
cmake --build build --config Release

# 3. Деплой Qt зависимостей
windeployqt --no-translations build/GMacros.exe
```

## Лицензия

Этот проект распространяется под лицензией **GNU General Public License v3.0** — см. [LICENSE](LICENSE).
