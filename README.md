# 42_Codexion
42_Codexion

```
42_Codexion/
├── README.md
└── coders/
    ├── codexion.h
    ├── Makefile
    ├── main.c
    ├── parsing.c
    ├── init.c
    ├── utils.c
    ├── actions.c
    ├── routine.c
    ├── monitor.c
    └── heap.c
```


Los 8 parámetros
``
./codexion 4    800    200         200        200          5                50              fifo
            │    │      │           │           │           │                │               │
            │    │      │           │           │           │                │               └─ scheduler
            │    │      │           │           │           │                └─ dongle_cooldown (ms)
            │    │      │           │           │           └─ number_of_compiles_required
            │    │      │           │           └─ time_to_refactor (ms)
            │    │      │           └─ time_to_debug (ms)
            │    │      └─ time_to_compile (ms)
            │    └─ time_to_burnout (ms)
            └─ number_of_coders
```

Qué significa cada uno en este ejemplo
4 — hay 4 coders y 4 dongles en la mesa circular.
800 — si un coder no empieza a compilar en 800ms desde su última compilación, muere.
200 — compilar tarda 200ms, durante ese tiempo tiene los dos dongles.
200 — debuggear tarda 200ms, sin dongles.
200 — refactorizar tarda 200ms, sin dongles.
5 — la simulación termina con éxito cuando todos han compilado 5 veces.
50 — después de soltar un dongle, ese dongle no puede usarse durante 50ms.
fifo — cuando varios coders esperan el mismo dongle, gana el que llegó primero.