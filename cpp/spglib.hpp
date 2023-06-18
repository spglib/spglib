#pragma once

#include <array>
#include <exception>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace Spglib {
	/**
	 * Get the version of the Spglib C++ binding
	 *
	 * @return X.Y.Z version
	 */
	const std::string version();
	/**
	 * Get the full version of the Spglib C++ binding
	 *
	 * @return full version
	 */
	const std::string version_full();
	/**
	 * Get the commit of the Spglib C++ binding
	 *
	 * @return commit
	 */
	const std::string commit();
	/**
	 * C++ exception object for spglib related errors
	 */
	class SpglibError : public std::exception {
		const std::string errorMsg;

	public:
		const char* what() const noexcept override;
		SpglibError(std::string&& message);
	};
	class Symmetry {
		// TODO: Find better interface compatible with std::mdspan
		std::array<double, 9> rotations;
		std::array<double, 3> translations;
	};
	class MagneticSymmetry : public Symmetry {
		bool time_reversal;
	};
	class Atom {
		std::array<double, 3> position;
		int type;
	};
	// TODO: Better name?
	class Lattice {
		std::array<double, 9> lattice;
		std::vector<Atom> atoms;
		// TODO: Allow for non-periodic dimensions
	};
	class PrimitiveLattice : public Lattice {
		Lattice& original_lattice;
	};
	class Spacegroup {
		int number;
		int hall_number;
		std::string international_symbol;
		std::string hall_symbol;
		std::string schoenflies;
		std::string pointgroup_international;
		std::string pointgroup_schoenflies;
		int arithmetic_crystal_class_number;
		std::string arithmetic_crystal_class_symbol;
		// TODO: Unclear what this is referencing
		char choice[6];
	};
	// TODO: should it inherit/reference Spacegroup
	class MagneticSpacegroup {
		int number;
		int type;
		int uni_number;
		int litvin_number;
		std::string bns_number;
		std::string og_number;
	};
	// TODO: Combine/templetize
	class Dataset {
		std::vector<Symmetry> symmetries;
		Spacegroup spacegroup;
		// These are dependent on the input, should be moved to constructor?
		std::array<double, 9> transformation_matrix;
		std::array<double, 3> origin_shift;
		// Smart reference to the input used
		std::shared_ptr<Lattice> lattice;
	};
	class MagneticDataset {
		std::vector<MagneticSymmetry> symmetries;
		MagneticSpacegroup spacegroup;
		// These are dependent on the input, should be moved to constructor?
		std::array<double, 9> transformation_matrix;
		std::array<double, 3> origin_shift;
		// Smart reference to the input used
		std::shared_ptr<Lattice> lattice;
	};
}// namespace Spglib
