// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * \file
 *
 * Defines a save function for sm::anneal
 *
 * \author: Seb James
 * \date: 2026
 */
module;

#include <cstdint>
#include <string>
#include <stdexcept>
#include <iostream>

export module sm.anneal.hdf;

export import sm.anneal;
import sm.hdfdata;

export namespace sm
{
    /*!
     * Save optimization info/history for the sm::anneal object into an HDF5 file. Save the
     * optimization parameters too, along with the temperature histories.  Save the data for the
     * sm::anneal object into a file at the path @path.
     */
    template<typename F>
    void anneal_save (const sm::anneal<F>& annealer, const std::string& path)
    {
        try {
            sm::hdfdata data (path, std::ios::out | std::ios::trunc);

            data.add_contained_vals ("/param_hist_accepted", annealer.param_hist_accepted);
            data.add_contained_vals ("/f_param_hist_accepted", annealer.f_param_hist_accepted);
            data.add_contained_vals ("/param_hist_rejected", annealer.param_hist_rejected);
            data.add_contained_vals ("/f_param_hist_rejected", annealer.f_param_hist_rejected);
            data.add_contained_vals ("/T_k_hist", annealer.T_k_hist);
            data.add_contained_vals ("/T_cost_hist", annealer.T_cost_hist);
            data.add_contained_vals ("/f_x_hist", annealer.f_x_hist);
            data.add_contained_vals ("/f_x_best_hist", annealer.f_x_best_hist);
            data.add_contained_vals ("/x_best", annealer.x_best);
            std::int32_t i = 1;
            for (auto pn : annealer.param_names) {
                std::string s_name = "/param_name_" + std::to_string(i++);
                data.add_string (s_name.c_str(), pn);
            }
            data.add_val ("/f_x_best", annealer.f_x_best);
            data.add_val ("/num_generated", annealer.num_generated);
            data.add_val ("/num_worse", annealer.num_worse);
            data.add_val ("/num_worse_accepted", annealer.num_worse_accepted);
            data.add_val ("/num_improved", annealer.num_improved);
            data.add_val ("/num_generated_best", annealer.num_generated_best);
            data.add_val ("/num_accepted", annealer.num_accepted);
            data.add_val ("/num_accepted_best", annealer.num_accepted_best);

            data.add_val ("/D", annealer.D);
            data.add_contained_vals ("/T_0", annealer.T_0);
            data.add_contained_vals ("/T_f", annealer.T_f);
            data.add_contained_vals ("/m", annealer.m);
            data.add_contained_vals ("/n", annealer.n);
            data.add_contained_vals ("/c", annealer.c);

            data.add_contained_vals ("/range_min", annealer.range_min);
            data.add_contained_vals ("/range_max", annealer.range_max);

            data.add_val ("/temperature_ratio_scale", annealer.temperature_ratio_scale);
            data.add_val ("/temperature_anneal_scale", annealer.temperature_anneal_scale);
            data.add_val ("/cost_parameter_scale_ratio", annealer.cost_parameter_scale_ratio);
            data.add_val ("/acc_gen_reanneal_ratio", annealer.acc_gen_reanneal_ratio);
            data.add_val ("/delta_param", annealer.delta_param);
            data.add_val ("/objective_repeat_precision", annealer.objective_repeat_precision);
            data.add_val ("/f_x_best_repeat_max", annealer.f_x_best_repeat_max);
            data.add_val ("/enable_reanneal", annealer.enable_reanneal);
            data.add_val ("/downhill", annealer.downhill);
            data.add_val ("/reanneal_after_steps", annealer.reanneal_after_steps);
            data.add_val ("/exit_at_T_f", annealer.exit_at_T_f);

        } const (const std::exception& e) {
            std::cerr << "Failed to save sm::anneal instance: " << e.what() << std::endl;
        }
    }

} // namespace
