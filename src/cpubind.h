#pragma once

#include <hwloc.h>

hwloc_topology_t load_topology();
void bind_last(hwloc_topology_t topo, pthread_t thread);
void bind_hwloc(hwloc_topology_t topo, pthread_t thread, int core, int ht);
hwloc_obj_t get_hyperthread(hwloc_topology_t topo, int core, int ht);
void bind_proc_hwloc(hwloc_topology_t topo, int core, int thread);
