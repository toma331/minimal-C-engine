# Minimal OpenGL Engine (Учебный проект)

Небольшой учебный движок на C с использованием OpenGL (fixed-function pipeline) и GLFW, без шейдеров.

## Что уже есть
- Окно и контекст OpenGL через GLFW.
- Камера с `yaw/pitch`, перемещение WASD + Q/E, мышь для вращения.
- Временной шаг `delta_time` для независимой от FPS анимации.
- Простая отладочная отрисовка (оси, сетка, FPS).
- Минимальная загрузка `OBJ` (пример: `obj/suzanne.obj`).
- Базовое fixed-function освещение (`GL_LIGHT0`) и нормали для `OBJ`.

## Управление
- `W/A/S/D` — движение по плоскости.
- `Q/E` — вниз/вверх.
- Мышь — вращение камеры.
- `Esc` — выход.

## Сборка и запуск (Linux)
```bash
make
./engine
```

## Примечания
- Это учебный проект.
- В проекте используются fixed‑function вызовы OpenGL 2.1 (`gluPerspective`, `gluLookAt`, `glRotatef`).
