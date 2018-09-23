#
# Copyright (C) 2013-2018 The ESPResSo project
#
# This file is part of ESPResSo.
#
# ESPResSo is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ESPResSo is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Handling of electrostatics

from __future__ import print_function, absolute_import
include "myconfig.pxi"
from espressomd.system cimport *
from espressomd.utils cimport *
from espressomd.electrostatics cimport *
from libcpp cimport vector
from libcpp.queue cimport queue
from libcpp.list cimport list
from libcpp.set cimport set
from utils cimport Vector3d
from cython.operator cimport dereference as deref, preincrement as inc
from libcpp cimport bool
from libcpp.memory cimport unique_ptr


cdef extern from "PartCfg.hpp":
    cppclass PartCfg:
        pass

cdef extern from "partCfg_global.hpp":
    PartCfg & partCfg()

cdef extern from "electrostatics_magnetostatics/iccShape.hpp":
    struct NewParticle:
        int parentID
        int iccTypeID
        int typeID
        Vector3d pos
        Vector3d normal
        Vector3d displace
        double area
        double eps
        double sigma
        double charge


IF ELECTROSTATICS and P3M:

    cdef extern from "electrostatics_magnetostatics/elc.hpp":
        ctypedef struct ELC_struct:
            double maxPWerror
            double gap_size
            double far_cut
            int neutralize
            double delta_mid_top,
            double delta_mid_bot,
            int const_pot,
            double pot_diff

        int ELC_set_params(double maxPWerror, double min_dist, double far_cut,
                           int neutralize, double delta_mid_top, double delta_mid_bot, int const_pot, double pot_diff)

        # links intern C-struct with python object
        ELC_struct elc_params

    cdef extern from "electrostatics_magnetostatics/icc.hpp":
        ctypedef struct iccp3m_struct:
            int numMissingIDs
            int largestID
            int n_ic
            int num_iteration
            double eout
            vector[double] areas
            vector[double] ein
            vector[double] sigma
            double convergence
            vector[Vector3d] normals
            Vector3d ext_field
            double relax
            int citeration
            int first_id

        ctypedef struct iccp3m_data_struct:
            NewParticle reducedPart
            list[vector[int]] trackList
            queue[vector[NewParticle]] newParticleData
            vector[double] iccCharges
            set[int] missingIDs
            double maxCharge
            double minCharge

        # links intern C-struct with python object
        iccp3m_struct iccp3m_cfg
        iccp3m_data_struct iccp3m_data

        void iccp3m_alloc_lists()

        void c_splitParticles(PartCfg & partCfg)

        void c_reduceParticle()

        void c_checkSet(int ID)

        void c_rebuildData(PartCfg & partCfg)

        void c_getCharges(PartCfg & partCfg)

        int c_addTypeWall(Vector3d normal, double dist, Vector3d cutoff, bool useTrans, double transMatrix[9], double invMatrix[9])

    cdef extern from "communication.hpp":
        int mpi_iccp3m_init()
        int mpi_iccp3m_iteration()
