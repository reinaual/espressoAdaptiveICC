#ifndef CELLS_H
#define CELLS_H
/** \file cells.h
 *
 *  This file contains everything related to the link cell
 *  algorithm. 
 *
 *  <b>Responsible:</b>
 *  <a href="mailto:limbach@mpip-mainz.mpg.de">Hanjo</a>
 *
 *  This modul strongly interacts with the ghost particle
 *  structure (ghosts.c) and the verlet list algorithm (verlet.c). The
 *  initialization (cells_init()) and cleaning up (cells_exit()) is
 *  called from the integrator (integrate.c).
 *
 *  The domain of a node is split into a 3D cell grid with dimension
 *  cell_grid. Together with one ghost cell layer on each side the
 *  overall dimension of the ghost cell grid is ghost_cell_grid.
 *  You can see a 2D graphical representation of the linked cell grid below. 
 *
 *  \image html linked_cells.gif "Linked cells structure"
 *  \image latex linked_cells.eps "Linked cells structure" \width=6cm
 *
 *  2D representation of a linked cell grid: n_cells = 64, cell_grid =
 *  {4,4}, ghost_cell_grid = {6,6}
 *
 * Each cell has 3^D-1 neighbour cells (For cell 14 they are
 * marked). Since we deal with pair forces, it is sufficient to
 * calculate only half of the interactions (Newtons law: actio =
 * reactio). I have chosen the upper half e.g. all neighbour cells with
 * a higher linear index (For cell 14 they are marked in light
 * blue). Caution: This implementation needs double sided ghost
 * communication! For single sided ghost communication one would need
 * some ghost-ghost cell interaction as well, which we do not need!
 *
 *  For more information on cells,
 *  see \ref cells.c "cells.c"
 */

/************************************************
 * data types
 ************************************************/

/** link cell structure. */
typedef struct {
  /** number of interacting neighbour cells . 
      A word about the interacting neighbour cells:\\
      In a 3D lattice each cell has 26 neighbours. Since we deal 
      with pair forces, it is sufficient to calculate only half 
      of the interactions (Newtons law: actio = reactio). For each 
      cell 13 neighbours. This has only to be done for the inner 
      cells. Caution: This implementation needs double sided ghost 
      communication! For single sided ghost communication one 
      would need some ghost-ghost cell interaction as well, which 
      we do not need! */
  int n_neighbours;
  /** interacting neighbour cell list (linear indices) */
  int *neighbours;

  /** number of particles in the cell. */
  int n_particles;
  /** size of particle index array. */
  int max_particles;
  /** particle index array (local index!). */
  int *particles;
} Cell;

/************************************************
 * exported variables
 ************************************************/

/** linked cell grid in nodes spatial domain. */
extern int cell_grid[3];
/** linked cell grid with ghost frame. */
extern int ghost_cell_grid[3];

/** number of linked cells (inner+ghosts). */
extern int n_cells;
/** linked cell list. */
extern Cell *cells;

/************************************************
 * functions
 ************************************************/

/** initialize link cell structures. 
 *
 *  cells_init calculates the cell grid dimension (cell_grid[3],
 *  ghost_cell_grid[3]) and allocates the space for the cell
 *  structure: cells.  Further it calculates the values for:
 *  cell_size[3], inv_cell_size[3], n_cells, n_inner_cells.
 *
 *  At the moment it also calculates the edges of the nodes
 *  spatial domain: my_left[3] and my_right[3].
 *
 *  Then it allocates space for the particle index list of each cell
 *  (cells[i].particles) with size PART_INCREMENT and initializes the
 *  neighbour list for the cells (init_cell_neighbours()).  
 */
void cells_init();

/** sort all particles into inner cells (no ghosts!). 
 *
 *  In order to build the verlet list (verlet.c) from the link cell
 *  structure one has to sort the particles into the cells. This is
 *  done after the particle exchange (exchange_part()).  
 *
 *  Sorting: Go through local particle list (0...n_particles) and
 *  store the local index into the particle list of the corresponding
 *  cell. 
 *
 *  cell particle index buffer (cells[i].particles) is reallocated if
 *  necessary. ATTENTION: there is at the moment no routine that
 *  reduces the size of this list if it would be possible. You have to
 *  use cells_exit() for that and than reinitialize the cell structure
 *  with cells_init() again. */
void sort_particles_into_cells();

/** exit link cell structures.  
    free space for linked cell structure.  */
void cells_exit();

/** realloc cell particle array.
    Step size for increase and decrease is PART_INCREMENT.
    @param index    linear cell index.
    @param size     desired size of the  particle array.
 */
void realloc_cell_particles(int index, int size);

#endif
