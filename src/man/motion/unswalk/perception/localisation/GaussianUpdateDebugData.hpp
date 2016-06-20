/*
 * GaussianUpdateDebugData.hpp
 *
 *  Created on: 03/05/2014
 *      Author: osushkov
 */

#pragma once

#include "LocalisationUtils.hpp"

#include <Eigen/Eigen>
#include <iostream>

struct GaussianUpdateDebugData {
   double preUpdateWeight;
   Eigen::MatrixXd preUpdateMean;
   Eigen::MatrixXd preUpdateCovariance;
   
   Eigen::MatrixXd updateInnovation;
   Eigen::MatrixXd updateJacobian;
   Eigen::MatrixXd updateObservationVariance;
   
   Eigen::MatrixXd combinedCovariance;
   Eigen::MatrixXd combinedCovarianceInv;
   Eigen::MatrixXd kalman;
   
   Eigen::MatrixXd postUpdateMean;
   Eigen::MatrixXd postUpdateCovariance;
   
   double weightAdjustment;
   double postUpdateWeight;
   
   void output(void) const {
      std::cout << "preUpdateWeight: " << preUpdateWeight << std::endl;
      
      prettyOutputMatrix("preUpdateMean", preUpdateMean);
      prettyOutputMatrix("preUpdateCovariance", preUpdateCovariance);
      
      prettyOutputMatrix("updateInnovation", updateInnovation);
      prettyOutputMatrix("updateJacobian", updateJacobian);
      prettyOutputMatrix("updateObservationVariance", updateObservationVariance);
      
      prettyOutputMatrix("combinedCovariance", combinedCovariance);
      prettyOutputMatrix("combinedCovarianceInv", combinedCovarianceInv);
      prettyOutputMatrix("kalman", kalman);
      
      prettyOutputMatrix("postUpdateMean", postUpdateMean);
      prettyOutputMatrix("postUpdateCovariance", postUpdateCovariance);
      
      std::cout << "weightAdjustment: " << weightAdjustment << std::endl;
      std::cout << "postUpdateWeight: " << postUpdateWeight << std::endl;
   }
};
