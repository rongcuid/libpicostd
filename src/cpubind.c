#include "cpubind.h"

#include "priv/utils.h"

hwloc_topology_t load_topology() {
  hwloc_topology_t topo;
  expect(hwloc_topology_init(&topo), "Failed to init topology\n");
  expect(
      hwloc_topology_set_flags(topo, HWLOC_TOPOLOGY_FLAG_DONT_CHANGE_BINDING),
      "Failed to set topology flags\n");
  expect(hwloc_topology_load(topo), "Failed to load topology\n");
  return topo;
}

hwloc_obj_t get_hyperthread(hwloc_topology_t topo, int core, int ht) {
  hwloc_obj_t core_obj = hwloc_get_obj_by_type(topo, HWLOC_OBJ_CORE, core);
  if (!core_obj) {
    fprintf(stderr, "Failed to get core\n");
    return nullptr;
  }
  hwloc_obj_t ht_obj =
      hwloc_get_obj_below_by_type(topo, HWLOC_OBJ_CORE, core, HWLOC_OBJ_PU, ht);
  if (!ht_obj) {
    fprintf(stderr, "Failed to get hyperthread [%d]\n", ht);
    return nullptr;
  }
  return ht_obj;
}

void bind_last(hwloc_topology_t topo, pthread_t thread) {
  // From HWLoc example
  int depth = hwloc_get_type_or_below_depth(topo, HWLOC_OBJ_CORE);
  hwloc_obj_t obj = hwloc_get_obj_by_depth(
      topo, depth, hwloc_get_nbobjs_by_depth(topo, depth) - 1);
  if (obj) {
    /* Get a copy of its cpuset that we may modify. */
    hwloc_cpuset_t cpuset = hwloc_bitmap_dup(obj->cpuset);

    /* Get only one logical processor (in case the core is
       SMT/hyper-threaded). */
    hwloc_bitmap_singlify(cpuset);

    /* And try to bind ourself there. */
    if (hwloc_set_thread_cpubind(topo, thread, cpuset, HWLOC_CPUBIND_STRICT)) {
      char *str;
      int error = errno;
      hwloc_bitmap_asprintf(&str, obj->cpuset);
      printf("Couldn't bind to cpuset %s: %s\n", str, strerror(error));
      free(str);
    }

    /* Free our cpuset copy */
    hwloc_bitmap_free(cpuset);
  }
}

void bind_hwloc(hwloc_topology_t topo, pthread_t thread, int core, int ht) {
  fprintf(stderr, "Setting pthread [0x%lx] to logical core [%d] ht [%d]\n",
          thread, core, ht);
  hwloc_obj_t ht_obj = get_hyperthread(topo, core, ht);
  if (ht_obj == nullptr) {
    fprintf(stderr, "Failed to get hyperthread\n");
    return;
  }
  expect(hwloc_set_thread_cpubind(topo, thread, ht_obj->cpuset,
                                  HWLOC_CPUBIND_STRICT),
         "Failed to bind pthread [0x%lx] to logical core [%d] ht [%d] "
         "(physical [%d])",
         thread, core, ht, ht_obj->os_index);
  fprintf(stderr, "Bound to core [%d(%d)] hyperthread [%d(%d)]\n", core,
          ht_obj->parent->os_index, ht, ht_obj->os_index);
}

void bind_proc_hwloc(hwloc_topology_t topo, int core, int thread) {
  pid_t pid = getpid();
  fprintf(stderr, "Setting PID [%d] to logical core [%d] ht [%d]\n", pid, core,
          thread);
  hwloc_obj_t ht_obj = get_hyperthread(topo, core, thread);
  if (ht_obj == nullptr) {
    fprintf(stderr, "Failed to get hyperthread\n");
    return;
  }
  expect(
      hwloc_set_proc_cpubind(topo, pid, ht_obj->cpuset, HWLOC_CPUBIND_STRICT),
      "Failed to bind PID [%d] to logical core [%d] ht [%d] (physical [%d])",
      thread, core, thread, ht_obj->os_index);
  fprintf(stderr, "Bound to core [%d(%d)] hyperthread [%d(%d)]\n", core,
          ht_obj->parent->os_index, thread, ht_obj->os_index);
}