/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | foam-extend: Open Source CFD
   \\    /   O peration     | Version:     4.0
    \\  /    A nd           | Web:         http://www.foam-extend.org
     \\/     M anipulation  | For copyright notice see file Copyright
-------------------------------------------------------------------------------
License
	This file is part of foam-extend.

	foam-extend is free software: you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation, either version 3 of the License, or (at your
	option) any later version.

	foam-extend is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

Class
	Foam::OutputFilterFunctionObject

Description
	A functionObject wrapper around OutputFilter to allow them to be
	created via the functions list within controlDict.

Note
	Since the timeIndex is used directly from Foam::Time, it is unaffected
	by user-time conversions. For example, Foam::engineTime might cause @a
	writeInterval to be degrees crank angle, but the functionObject
	execution @a interval would still be in timestep.

SourceFiles
	OutputFilterFunctionObject.C

\*---------------------------------------------------------------------------*/

#ifndef OutputFilterFunctionObject_H
#define OutputFilterFunctionObject_H

#include "functionObject.H"
#include "dictionary.H"
#include "outputFilterOutputControl.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


template<class OutputFilter>
class OutputFilterFunctionObject
:
	public functionObject
{
	// Private data

		//- Reference to the time database
		const Time& time_;

		//- Input dictionary
		dictionary dict_;

		//- Name of region
		word regionName_;

		//- Optional dictionary name to supply required inputs
		word dictName_;

		//- Switch for the execution of the functionObject
		bool enabled_;

		//- Switch to store filter in between writes or use on-the-fly
		//  construction
		bool storeFilter_;

		//- Output controls
		outputFilterOutputControl outputControl_;

		//- Pointer to the output filter
		autoPtr<OutputFilter> ptr_;


	// Private Member Functions

		//- Read relevant dictionary entries
		void readDict();

		//- Creates most of the data associated with this object.
		void allocateFilter();

		//- Destroys most of the data associated with this object.
		void destroyFilter();

		//- Disallow default bitwise copy construct
		OutputFilterFunctionObject(const OutputFilterFunctionObject&);

		//- Disallow default bitwise assignment
		void operator=(const OutputFilterFunctionObject&);


public:

	//- Runtime type information
	TypeName(OutputFilter::typeName_());


	// Constructors

		//- Construct from components
		OutputFilterFunctionObject
		(
			const word& name,
			const Time&,
			const dictionary&
		);


	// Member Functions

		// Access

			//- Return time database
			virtual const Time& time() const
			{
				return time_;
			}

			//- Return the input dictionary
			virtual const dictionary& dict() const
			{
				return dict_;
			}

			//- Return the region name
			virtual const word& regionName() const
			{
				return regionName_;
			}

			//- Return the optional dictionary name
			virtual const word& dictName() const
			{
				return dictName_;
			}

			//- Return the enabled flag
			virtual bool enabled() const
			{
				return enabled_;
			}

			//- Return the output control object
			virtual const outputFilterOutputControl& outputControl() const
			{
				return outputControl_;
			}

			//- Return the output filter
			virtual const OutputFilter& outputFilter() const
			{
				return ptr_();
			}


		// Function object control

			//- Switch the function object on
			virtual void on();

			//- Switch the function object off
			virtual void off();


			//- Called at the start of the time-loop
			virtual bool start();

			//- Called at each ++ or += of the time-loop
			virtual bool execute();

			//- Called when Time::run() determines that the time-loop exits
			virtual bool end();


			//- Read and set the function object if its data have changed
			virtual bool read(const dictionary&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#	include "OutputFilterFunctionObject.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
