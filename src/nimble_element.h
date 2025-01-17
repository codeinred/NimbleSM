/*
//@HEADER
// ************************************************************************
//
//                                NimbleSM
//                             Copyright 2018
//   National Technology & Engineering Solutions of Sandia, LLC (NTESS)
//
// Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
// retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY NTESS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
// NO EVENT SHALL NTESS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions?  Contact David Littlewood (djlittl@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#ifndef NIMBLE_ELEMENT_H
#define NIMBLE_ELEMENT_H

#include "nimble_defs.h"
#include "nimble_macros.h"
#include "nimble_utils.h"

#include <vector>

namespace nimble {

/// \brief Abstract class for representing an element
class Element
{
 public:
  /// \brief Default constructor
  NIMBLE_FUNCTION
  Element() = default;

  /// \brief Default virtual destructor
  NIMBLE_FUNCTION
  virtual ~Element() = default;

  /// \brief Virtual function returning the spatial dimension
  /// \return Spatial dimension
  virtual int
  Dim() const = 0;

  /// \brief Virtual function returning the number of nodes in the element
  /// \return Number of nodes in the element
  virtual int
  NumNodesPerElement() const = 0;

  /// \brief Virtual function returning the number of integration points per element
  /// \return Number of integration points per element
  virtual int
  NumIntegrationPointsPerElement() const = 0;

  /// \brief Virtual function to compute the lumped mass
  ///
  /// \param[in] density  Material density
  /// \param[in] node_reference_coords  Pointer to array of nodal reference coordinates
  /// \param[out] lumped_mass   Pointer to array of nodal lumped mass value
  virtual void
  ComputeLumpedMass(double density, const double* node_reference_coords, double* lumped_mass) const = 0;

#ifdef NIMBLE_HAVE_KOKKOS
  /// \brief Virtual function to compute the lumped mass
  ///
  /// \param[in] density  Material density
  /// \param[in] node_reference_coords  Pointer to array of nodal reference coordinates
  /// \param[out] lumped_mass   Pointer to array of nodal lumped mass value
  NIMBLE_FUNCTION
  virtual void
  ComputeLumpedMass(
      double                                         density,
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_reference_coords,
      nimble_kokkos::DeviceScalarNodeGatheredSubView lumped_mass) const = 0;
#endif

  /// \brief Virtual function to compute the characteristic length
  ///
  /// \param[in] node_coords  Pointer to array of nodal coordinates
  /// \return  Characteristic length
  virtual double
  ComputeCharacteristicLength(const double* node_coords) = 0;

  virtual void
  ComputeVolumeAverage(
      const double* node_current_coords,
      int           num_quantities,
      const double* int_pt_quantities,
      double&       volume,
      double*       volume_averaged_quantity) const = 0;

#ifdef NIMBLE_HAVE_KOKKOS

  NIMBLE_FUNCTION
  virtual void
  ComputeVolume(
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_reference_coords,
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_displacements,
      nimble_kokkos::DeviceScalarElemSingleEntryView elem_volume) const = 0;

  NIMBLE_FUNCTION
  virtual void
  ComputeVolumeAverageSymTensor(
      nimble_kokkos::DeviceVectorNodeGatheredSubView    node_reference_coords,
      nimble_kokkos::DeviceVectorNodeGatheredSubView    node_displacements,
      nimble_kokkos::DeviceSymTensorIntPtSubView        int_pt_quantities,
      nimble_kokkos::DeviceSymTensorElemSingleEntryView vol_ave_quantity) const = 0;

  NIMBLE_FUNCTION
  virtual void
  ComputeVolumeAverageFullTensor(
      nimble_kokkos::DeviceVectorNodeGatheredSubView     node_reference_coords,
      nimble_kokkos::DeviceVectorNodeGatheredSubView     node_displacements,
      nimble_kokkos::DeviceFullTensorIntPtSubView        int_pt_quantities,
      nimble_kokkos::DeviceFullTensorElemSingleEntryView vol_ave_quantity) const = 0;
#endif

  /// \brief Virtual function to compute the deformation gradients
  ///
  /// \param[in] node_reference_coords  Pointer to array of reference nodal coordinates
  /// \param[in] node_current_coords  Pointer to array of current nodal coordinates
  /// \param[out] deformation_gradients  Pointer to array of deformation gradients
  virtual void
  ComputeDeformationGradients(
      const double* node_reference_coords,
      const double* node_current_coords,
      double*       deformation_gradients) const = 0;

#ifdef NIMBLE_HAVE_KOKKOS
  /// \brief Virtual function to compute the deformation gradients
  ///
  /// \param[in] node_reference_coords  Kokkos view of reference nodal coordinates
  /// \param[in] node_displacements  Kokkos view of nodal displacements
  /// \param[out] deformation_gradients  Kokkos view of deformation gradients
  NIMBLE_FUNCTION
  virtual void
  ComputeDeformationGradients(
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_reference_coords,
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_displacements,
      nimble_kokkos::DeviceFullTensorIntPtSubView    deformation_gradients) const = 0;
#endif

  /// \brief Virtual function to compute the tangent
  ///
  /// \param[in] node_current_coords  Pointer to array of current nodal coordinates
  /// \param[in] material_tangent  Pointer to array of material tangent
  /// \param[out] tangent  Pointer to array of tangent
  virtual void
  ComputeTangent(const double* node_reference_coords, const double* material_tangent, double* tangent) = 0;

  /// \brief Virtual function to compute the nodal forces
  ///
  /// \param[in] node_current_coords  Pointer to array of current nodal coordinates
  /// \param[in] int_pt_stresses  Pointer to array of stresses at integration points
  /// \param[out] node_forces  Pointer to array of forces
  virtual void
  ComputeNodalForces(const double* node_current_coords, const double* int_pt_stresses, double* node_forces) = 0;

#ifdef NIMBLE_HAVE_KOKKOS
  /// \brief Virtual function to compute the nodal forces
  ///
  /// \param[in] node_reference_coords  Kokkos view of reference nodal coordinates
  /// \param[in] node_displacements  Kokkos view of nodal displacements
  /// \param[in] int_pt_stresses  Kokkos view of stress at integration points
  /// \param[out] node_forces  Kokkos view of nodal forces
  NIMBLE_FUNCTION
  virtual void
  ComputeNodalForces(
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_reference_coords,
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_displacements,
      nimble_kokkos::DeviceSymTensorIntPtSubView     int_pt_stresses,
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_forces) const = 0;
#endif

 protected:
  const int K_S_XX_ = 0;
  const int K_S_YY_ = 1;
  const int K_S_ZZ_ = 2;
  const int K_S_XY_ = 3;
  const int K_S_YZ_ = 4;
  const int K_S_ZX_ = 5;
  const int K_S_YX_ = 3;
  const int K_S_ZY_ = 4;
  const int K_S_XZ_ = 5;

  const int K_F_XX_ = 0;
  const int K_F_YY_ = 1;
  const int K_F_ZZ_ = 2;
  const int K_F_XY_ = 3;
  const int K_F_YZ_ = 4;
  const int K_F_ZX_ = 5;
  const int K_F_YX_ = 6;
  const int K_F_ZY_ = 7;
  const int K_F_XZ_ = 8;
};

/// \brief Class for an 8-node hexahedral element
class HexElement : public Element
{
 private:
  static constexpr int dim_         = 3;
  static constexpr int num_nodes_   = 8;
  static constexpr int num_int_pts_ = 8;

 public:
  /// \brief Default constructor
  NIMBLE_FUNCTION
  HexElement();

  /// \brief Default destructor
  NIMBLE_FUNCTION
  ~HexElement() override = default;

  /// \brief Returns the spatial dimension
  /// \return Spatial dimension (3 for the hexahedral element)
  int
  Dim() const override
  {
    return dim_;
  }

  /// \brief Returns the number of nodes per element
  /// \return Number of nodes per element (8 for the Q1 hexahedral element)
  int
  NumNodesPerElement() const override
  {
    return num_nodes_;
  }

  /// \brief Returns the number of integration points per element
  /// \return Number of integration points per element (8 for the Q1 hexahedral element)
  int
  NumIntegrationPointsPerElement() const override
  {
    return num_int_pts_;
  }

 protected:
  template <class ViewT>
  void
  ComputeConsistentMass_impl(
      double      density,
      const ViewT node_reference_coords,
      double      consistent_mass_matrix[][num_nodes_]) const
  {
    double jac_det[num_int_pts_];
    double rc1, rc2, rc3, sfd1, sfd2, sfd3;
    for (int int_pt = 0; int_pt < num_int_pts_; int_pt++) {
      // \sum_{i}^{N_{node}} x_{i} \frac{\partial N_{i} (\xi)}{\partial \xi}
      double a[][dim_]     = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
      double a_inv[][dim_] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

      for (int n = 0; n < num_nodes_; n++) {
        rc1  = node_reference_coords(n, 0);
        rc2  = node_reference_coords(n, 1);
        rc3  = node_reference_coords(n, 2);
        sfd1 = shape_fcn_deriv_[24 * int_pt + dim_ * n];
        sfd2 = shape_fcn_deriv_[24 * int_pt + dim_ * n + 1];
        sfd3 = shape_fcn_deriv_[24 * int_pt + dim_ * n + 2];
        a[0][0] += rc1 * sfd1;
        a[0][1] += rc1 * sfd2;
        a[0][2] += rc1 * sfd3;
        a[1][0] += rc2 * sfd1;
        a[1][1] += rc2 * sfd2;
        a[1][2] += rc2 * sfd3;
        a[2][0] += rc3 * sfd1;
        a[2][1] += rc3 * sfd2;
        a[2][2] += rc3 * sfd3;
      }
      jac_det[int_pt] = Invert3x3(a, a_inv);
    }

    for (int i = 0; i < num_nodes_; i++) {
      for (int j = 0; j < num_nodes_; j++) {
        consistent_mass_matrix[i][j] = 0.0;
        for (int int_pt = 0; int_pt < num_int_pts_; int_pt++) {
          consistent_mass_matrix[i][j] += int_wts_[int_pt] * density * shape_fcn_vals_[int_pt * num_nodes_ + i] *
                                          shape_fcn_vals_[int_pt * num_nodes_ + j] * jac_det[int_pt];
        }
      }
    }
  }

 public:
  /// \brief Compute the lumped mass
  ///
  /// \param[in] density  Material density
  /// \param[in] node_reference_coords  Pointer to array of nodal reference coordinates
  /// \param[out] lumped_mass   Pointer to array of nodal lumped mass value
  void
  ComputeLumpedMass(double density, const double* node_reference_coords, double* lumped_mass) const override;

#ifdef NIMBLE_HAVE_KOKKOS
  /// \brief Compute the lumped mass when using Kokkos objects
  ///
  /// \param[in] density  Material density
  /// \param[in] node_reference_coords  Pointer to array of nodal reference coordinates
  /// \param[out] lumped_mass   Pointer to array of nodal lumped mass value
  NIMBLE_FUNCTION
  void
  ComputeLumpedMass(
      double                                         density,
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_reference_coords,
      nimble_kokkos::DeviceScalarNodeGatheredSubView lumped_mass) const override;
#endif

  /// \brief Function to compute the characteristic length
  ///
  /// \param[in] node_coords  Pointer to array of nodal coordinates
  /// \return  Characteristic length
  double
  ComputeCharacteristicLength(const double* node_coords) override;

 protected:

  template <class ConstViewVec, class ConstViewTensor, class ViewTensor>
  void
  ComputeVolumeAverageQuantities_impl(
      ConstViewVec    node_reference_coords,
      ConstViewVec    node_displacements,
      ConstViewTensor int_pt_quantities,
      ViewTensor      vol_ave_quantity,
      int num_quantities,
      double          &volume) const
  {
    double cc1, cc2, cc3, sfd1, sfd2, sfd3;
    double jac_det;
    double a_inv[][dim_] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

    std::vector<double> vol_ave(num_quantities, 0);
    volume = 0.0;

    for (int int_pt = 0; int_pt < num_int_pts_; int_pt++) {
      // \sum_{i}^{N_{node}} x_{i} \frac{\partial N_{i} (\xi)}{\partial \xi}
      double a[][dim_] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

      for (int n = 0; n < num_nodes_; n++) {
        cc1  = node_reference_coords(n, 0) + node_displacements(n, 0);
        cc2  = node_reference_coords(n, 1) + node_displacements(n, 1);
        cc3  = node_reference_coords(n, 2) + node_displacements(n, 2);
        sfd1 = shape_fcn_deriv_[24 * int_pt + dim_ * n];
        sfd2 = shape_fcn_deriv_[24 * int_pt + dim_ * n + 1];
        sfd3 = shape_fcn_deriv_[24 * int_pt + dim_ * n + 2];
        a[0][0] += cc1 * sfd1;
        a[0][1] += cc1 * sfd2;
        a[0][2] += cc1 * sfd3;
        a[1][0] += cc2 * sfd1;
        a[1][1] += cc2 * sfd2;
        a[1][2] += cc2 * sfd3;
        a[2][0] += cc3 * sfd1;
        a[2][1] += cc3 * sfd2;
        a[2][2] += cc3 * sfd3;
      }
      jac_det = Invert3x3(a, a_inv);
      volume += jac_det;
      for (int i = 0; i < num_quantities; ++i) {
        vol_ave[i] += int_pt_quantities(int_pt, i) * int_wts_[int_pt] * jac_det;
      }
    }

    for (int i = 0; i < num_quantities; ++i) {
      vol_ave[i] /= volume;
      vol_ave_quantity(i) = vol_ave[i];
    }
  }

 public:
  void
  ComputeVolumeAverage(
      const double* node_current_coords,
      int           num_quantities,
      const double* int_pt_quantities,
      double&       volume,
      double*       volume_averaged_quantity) const override;

#ifdef NIMBLE_HAVE_KOKKOS

  NIMBLE_FUNCTION
  void
  ComputeVolume(
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_reference_coords,
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_displacements,
      nimble_kokkos::DeviceScalarElemSingleEntryView elem_volume) const override;

  NIMBLE_FUNCTION
  void
  ComputeVolumeAverageSymTensor(
      nimble_kokkos::DeviceVectorNodeGatheredSubView    node_reference_coords,
      nimble_kokkos::DeviceVectorNodeGatheredSubView    node_displacements,
      nimble_kokkos::DeviceSymTensorIntPtSubView        int_pt_quantities,
      nimble_kokkos::DeviceSymTensorElemSingleEntryView vol_ave_quantity) const override;

  NIMBLE_FUNCTION
  void
  ComputeVolumeAverageFullTensor(
      nimble_kokkos::DeviceVectorNodeGatheredSubView     node_reference_coords,
      nimble_kokkos::DeviceVectorNodeGatheredSubView     node_displacements,
      nimble_kokkos::DeviceFullTensorIntPtSubView        int_pt_quantities,
      nimble_kokkos::DeviceFullTensorElemSingleEntryView vol_ave_quantity) const override;
#endif

 protected:
  template <class ConstViewT, class TensorViewT>
  NIMBLE_FUNCTION void
  ComputeDeformationGradients_impl(
      ConstViewT  node_reference_coords,
      ConstViewT  node_displacements,
      TensorViewT deformation_gradients) const
  {
    double rc1, rc2, rc3, cc1, cc2, cc3, sfd1, sfd2, sfd3;
    double b_inv[][3]    = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    double def_grad[][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

    // Loop over the integration points
    for (int int_pt = 0; int_pt < 8; int_pt++) {
      // \sum_{i}^{N_{node}} x_{i} \frac{\partial N_{i} (\xi)}{\partial \xi}
      double a[][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

      // \sum_{i}^{N_{node}} X_{i} \frac{\partial N_{i} (\xi)}{\partial \xi}
      double b[][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

      // Sum over the number of nodes
      for (int j = 0; j < 8; j++) {
        rc1 = node_reference_coords(j, 0);
        rc2 = node_reference_coords(j, 1);
        rc3 = node_reference_coords(j, 2);

        cc1 = node_reference_coords(j, 0) + node_displacements(j, 0);
        cc2 = node_reference_coords(j, 1) + node_displacements(j, 1);
        cc3 = node_reference_coords(j, 2) + node_displacements(j, 2);

        sfd1 = shape_fcn_deriv_[24 * int_pt + 3 * j];
        sfd2 = shape_fcn_deriv_[24 * int_pt + 3 * j + 1];
        sfd3 = shape_fcn_deriv_[24 * int_pt + 3 * j + 2];

        a[0][0] += cc1 * sfd1;
        a[0][1] += cc1 * sfd2;
        a[0][2] += cc1 * sfd3;
        a[1][0] += cc2 * sfd1;
        a[1][1] += cc2 * sfd2;
        a[1][2] += cc2 * sfd3;
        a[2][0] += cc3 * sfd1;
        a[2][1] += cc3 * sfd2;
        a[2][2] += cc3 * sfd3;

        b[0][0] += rc1 * sfd1;
        b[0][1] += rc1 * sfd2;
        b[0][2] += rc1 * sfd3;
        b[1][0] += rc2 * sfd1;
        b[1][1] += rc2 * sfd2;
        b[1][2] += rc2 * sfd3;
        b[2][0] += rc3 * sfd1;
        b[2][1] += rc3 * sfd2;
        b[2][2] += rc3 * sfd3;
      }

      Invert3x3(b, b_inv);

      for (int j = 0; j < 3; j++) {
        for (int k = 0; k < 3; k++) {
          def_grad[j][k] = a[j][0] * b_inv[0][k] + a[j][1] * b_inv[1][k] + a[j][2] * b_inv[2][k];
        }
      }

      deformation_gradients(int_pt, K_F_XX_) = def_grad[0][0];
      deformation_gradients(int_pt, K_F_XY_) = def_grad[0][1];
      deformation_gradients(int_pt, K_F_XZ_) = def_grad[0][2];
      deformation_gradients(int_pt, K_F_YX_) = def_grad[1][0];
      deformation_gradients(int_pt, K_F_YY_) = def_grad[1][1];
      deformation_gradients(int_pt, K_F_YZ_) = def_grad[1][2];
      deformation_gradients(int_pt, K_F_ZX_) = def_grad[2][0];
      deformation_gradients(int_pt, K_F_ZY_) = def_grad[2][1];
      deformation_gradients(int_pt, K_F_ZZ_) = def_grad[2][2];
    }
  }

 public:

  /// \brief Function to compute the deformation gradients
  ///
  /// \param[in] node_reference_coords  Pointer to array of reference nodal coordinates
  /// \param[in] node_current_coords  Pointer to array of current nodal coordinates
  /// \param[out] deformation_gradients  Pointer to array of deformation gradients
  void
  ComputeDeformationGradients(
      const double* node_reference_coords,
      const double* node_current_coords,
      double*       deformation_gradients) const override;

#ifdef NIMBLE_HAVE_KOKKOS
  /// \brief Function to compute the deformation gradients
  ///
  /// \param[in] node_reference_coords  Kokkos view of reference nodal coordinates
  /// \param[in] node_displacements  Kokkos view of current nodal coordinates
  /// \param[out] deformation_gradients  Kokkos view of deformation gradients
  NIMBLE_FUNCTION
  void
  ComputeDeformationGradients(
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_reference_coords,
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_displacements,
      nimble_kokkos::DeviceFullTensorIntPtSubView    deformation_gradients) const override;
#endif

  /// \brief Function to compute the tangent
  ///
  /// \param[in] node_current_coords  Pointer to array of current nodal coordinates
  /// \param[in] material_tangent  Pointer to array of material tangent
  /// \param[out] tangent  Pointer to array of tangent
  void
  ComputeTangent(const double* node_current_coords, const double* material_tangent, double* element_tangent) override;

 protected:
  template <class ConstViewT, class ViewTensorT, class ViewT>
  void
  ComputeNodalForces_impl(
      ConstViewT  node_reference_coords,
      ConstViewT  node_displacements,
      ViewTensorT int_pt_stresses,
      ViewT       node_forces) const
  {
    double cc1, cc2, cc3, sfd1, sfd2, sfd3, dN_dx1, dN_dx2, dN_dx3, f1, f2, f3;
    double jac_det;
    double a_inv[][dim_] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    double force[][dim_] = {
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}};
    constexpr int dim_nodes = dim_ * num_nodes_;

    for (int int_pt = 0; int_pt < num_int_pts_; int_pt++) {
      // \sum_{i}^{N_{node}} x_{i} \frac{\partial N_{i} (\xi)}{\partial \xi}
      double a[][dim_] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

      for (int n = 0; n < num_nodes_; n++) {
        cc1  = node_reference_coords(n, 0) + node_displacements(n, 0);
        cc2  = node_reference_coords(n, 1) + node_displacements(n, 1);
        cc3  = node_reference_coords(n, 2) + node_displacements(n, 2);
        sfd1 = shape_fcn_deriv_[dim_nodes * int_pt + dim_ * n];
        sfd2 = shape_fcn_deriv_[dim_nodes * int_pt + dim_ * n + 1];
        sfd3 = shape_fcn_deriv_[dim_nodes * int_pt + dim_ * n + 2];
        a[0][0] += cc1 * sfd1;
        a[0][1] += cc1 * sfd2;
        a[0][2] += cc1 * sfd3;
        a[1][0] += cc2 * sfd1;
        a[1][1] += cc2 * sfd2;
        a[1][2] += cc2 * sfd3;
        a[2][0] += cc3 * sfd1;
        a[2][1] += cc3 * sfd2;
        a[2][2] += cc3 * sfd3;
      }

      jac_det = Invert3x3(a, a_inv);

      for (int node = 0; node < num_nodes_; node++) {
        dN_dx1 = shape_fcn_deriv_[int_pt * dim_nodes + dim_ * node] * a_inv[0][0] +
                 shape_fcn_deriv_[int_pt * dim_nodes + dim_ * node + 1] * a_inv[1][0] +
                 shape_fcn_deriv_[int_pt * dim_nodes + dim_ * node + 2] * a_inv[2][0];

        dN_dx2 = shape_fcn_deriv_[int_pt * dim_nodes + dim_ * node] * a_inv[0][1] +
                 shape_fcn_deriv_[int_pt * dim_nodes + dim_ * node + 1] * a_inv[1][1] +
                 shape_fcn_deriv_[int_pt * dim_nodes + dim_ * node + 2] * a_inv[2][1];

        dN_dx3 = shape_fcn_deriv_[int_pt * dim_nodes + dim_ * node] * a_inv[0][2] +
                 shape_fcn_deriv_[int_pt * dim_nodes + dim_ * node + 1] * a_inv[1][2] +
                 shape_fcn_deriv_[int_pt * dim_nodes + dim_ * node + 2] * a_inv[2][2];

        f1 = dN_dx1 * int_pt_stresses(int_pt, K_S_XX_) + dN_dx2 * int_pt_stresses(int_pt, K_S_YX_) +
             dN_dx3 * int_pt_stresses(int_pt, K_S_ZX_);

        f2 = dN_dx1 * int_pt_stresses(int_pt, K_S_XY_) + dN_dx2 * int_pt_stresses(int_pt, K_S_YY_) +
             dN_dx3 * int_pt_stresses(int_pt, K_S_ZY_);

        f3 = dN_dx1 * int_pt_stresses(int_pt, K_S_XZ_) + dN_dx2 * int_pt_stresses(int_pt, K_S_YZ_) +
             dN_dx3 * int_pt_stresses(int_pt, K_S_ZZ_);

        f1 *= jac_det * int_wts_[int_pt];
        f2 *= jac_det * int_wts_[int_pt];
        f3 *= jac_det * int_wts_[int_pt];

        // profiling showed that calling the -= operator directly on the kokkos
        // view is expensive
        force[node][0] -= f1;
        force[node][1] -= f2;
        force[node][2] -= f3;
      }
    }

    for (int node = 0; node < num_nodes_; node++) {
      node_forces(node, 0) = force[node][0];
      node_forces(node, 1) = force[node][1];
      node_forces(node, 2) = force[node][2];
    }
  }

 public:

  /// \brief Virtual function to compute the nodal forces
  ///
  /// \param[in] node_current_coords  Pointer to array of current nodal coordinates
  /// \param[in] int_pt_stresses  Pointer to array of stresses at integration points
  /// \param[out] node_forces  Pointer to array of forces
  void
  ComputeNodalForces(const double* node_current_coords, const double* int_pt_stresses, double* node_forces) override;

#ifdef NIMBLE_HAVE_KOKKOS
  /// \brief Function to compute the nodal forces
  ///
  /// \param[in] node_reference_coords  Kokkos view of reference nodal coordinates
  /// \param[in] node_displacements  Kokkos view of nodal displacements
  /// \param[in] int_pt_stresses  Kokkos view of stress at integration points
  /// \param[out] node_forces  Kokkos view of nodal forces
  NIMBLE_FUNCTION
  void
  ComputeNodalForces(
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_reference_coords,
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_displacements,
      nimble_kokkos::DeviceSymTensorIntPtSubView     int_pt_stresses,
      nimble_kokkos::DeviceVectorNodeGatheredSubView node_forces) const override
  {
    ComputeNodalForces_impl(node_reference_coords, node_displacements, int_pt_stresses, node_forces);
  }
#endif

 protected:
  NIMBLE_FUNCTION
  void
  ShapeFunctionValues(const double* natural_coords, double* shape_function_values);

  NIMBLE_FUNCTION
  void
  ShapeFunctionDerivatives(const double* natural_coords, double* shape_function_derivatives);

 private:
  double int_pts_[num_int_pts_ * dim_]{};
  double int_wts_[num_int_pts_]{};
  double shape_fcn_vals_[num_nodes_ * num_int_pts_]{};
  double shape_fcn_deriv_[num_nodes_ * num_int_pts_ * dim_]{};
};

}  // namespace nimble

#endif  // NIMBLE_ELEMENT_H
