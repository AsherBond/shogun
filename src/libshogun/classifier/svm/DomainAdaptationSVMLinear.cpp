/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2007-2011 Christian Widmer
 * Copyright (C) 2007-2011 Max-Planck-Society
 */

#include "lib/config.h"

#ifdef HAVE_LAPACK

#include "classifier/svm/DomainAdaptationSVMLinear.h"
#include "lib/io.h"
#include "base/Parameter.h"
#include <iostream>
#include <vector>

using namespace shogun;


CDomainAdaptationSVMLinear::CDomainAdaptationSVMLinear() : CLibLinear(L2R_L1LOSS_SVC_DUAL)
{
	init(NULL, 0.0);
}


CDomainAdaptationSVMLinear::CDomainAdaptationSVMLinear(float64_t C, CDotFeatures* f, CLabels* lab, CLinearClassifier* pre_svm, float64_t B_param) : CLibLinear(C, f, lab)
{
	init(pre_svm, B_param);

}


CDomainAdaptationSVMLinear::~CDomainAdaptationSVMLinear()
{

	SG_UNREF(presvm);
	SG_DEBUG("deleting DomainAdaptationSVMLinear\n");
}


void CDomainAdaptationSVMLinear::init(CLinearClassifier* pre_svm, float64_t B_param)
{

	if (pre_svm)
	{
		// increase reference counts
		SG_REF(pre_svm);

		// set bias of parent svm to zero
		pre_svm->set_bias(0.0);
	}

	this->presvm = pre_svm;
	this->B = B_param;
	this->train_factor = 1.0;

	set_liblinear_solver_type(L2R_L1LOSS_SVC_DUAL);

	// invoke sanity check
	is_presvm_sane();

    // serialization code
	m_parameters->add((CSGObject**) &presvm, "presvm", "SVM to regularize against");
	m_parameters->add(&B, "B",  "Regularization strenth B.");
	m_parameters->add(&train_factor, "train_factor",  "train_factor");

}


bool CDomainAdaptationSVMLinear::is_presvm_sane()
{

	if (!presvm) {

		SG_WARNING("presvm is null");

	} else {

        if (presvm->get_bias() != 0) {
            SG_ERROR("presvm bias not set to zero");
        }

        if (presvm->get_features()->get_feature_type() != this->get_features()->get_feature_type()) {
            SG_ERROR("feature types do not agree");
        }
    }

	return true;

}


bool CDomainAdaptationSVMLinear::train(CDotFeatures* train_data)
{

	CDotFeatures* tmp_data;

	if (train_data)
	{
		if (labels->get_num_labels() != train_data->get_num_vectors())
			SG_ERROR("Number of training vectors does not match number of labels\n");
		tmp_data = train_data;

	} else {

		tmp_data = features;
	}

	int32_t num_training_points = get_labels()->get_num_labels();

	std::vector<float64_t> lin_term = std::vector<float64_t>(num_training_points);

    if (presvm)
    {
    	ASSERT(presvm->get_bias() == 0.0);

        // bias of parent SVM was set to zero in constructor, already contains B
        CLabels* parent_svm_out = presvm->classify(tmp_data);

        SG_DEBUG("pre-computing linear term from presvm\n");

        // pre-compute linear term
        for (int32_t i=0; i!=num_training_points; i++)
        {
            lin_term[i] = train_factor * B * get_label(i) * parent_svm_out->get_label(i) - 1.0;
        }

    	// set linear term for QP
    	this->set_linear_term(&lin_term[0], lin_term.size());

    }

	/*
	// warm-start liblinear
	//TODO test this code, measure speed-ups
    //presvm w stored in presvm
    float64_t* tmp_w;
    presvm->get_w(tmp_w, w_dim);

    //copy vector
    float64_t* tmp_w_copy = new float64_t[w_dim];
    std::copy(tmp_w, tmp_w + w_dim, tmp_w_copy);

	for (int32_t i=0; i!=w_dim; i++)
	{
		tmp_w_copy[i] = B * tmp_w_copy[i];
	}

	//set w (copied in setter)
    set_w(tmp_w_copy, w_dim);
    delete[] tmp_w_copy;
	*/

	bool success = false;

	//train SVM
	if (train_data)
	{
		success = CLibLinear::train(train_data);
	} else {
		success = CLibLinear::train();
	}

	//ASSERT(presvm)

	return success;

}


CLinearClassifier* CDomainAdaptationSVMLinear::get_presvm()
{
	return presvm;
}


float64_t CDomainAdaptationSVMLinear::get_B()
{
	return B;
}


float64_t CDomainAdaptationSVMLinear::get_train_factor()
{
	return train_factor;
}


void CDomainAdaptationSVMLinear::set_train_factor(float64_t factor)
{
	train_factor = factor;
}


CLabels* CDomainAdaptationSVMLinear::classify(CDotFeatures* data)
{

    ASSERT(presvm->get_bias()==0.0);

    int32_t num_examples = data->get_num_vectors();

    CLabels* out_current = CLibLinear::classify(data);

    if (presvm)
    {

        // recursive call if used on DomainAdaptationSVM object
        CLabels* out_presvm = presvm->classify(data);


        // combine outputs
        for (int32_t i=0; i!=num_examples; i++)
        {
            float64_t out_combined = out_current->get_label(i) + B*out_presvm->get_label(i);
            out_current->set_label(i, out_combined);
        }

    }


    return out_current;

}

#endif //HAVE_LAPACK

