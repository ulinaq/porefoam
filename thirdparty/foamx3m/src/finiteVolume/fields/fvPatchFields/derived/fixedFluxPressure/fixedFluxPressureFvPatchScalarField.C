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

\*---------------------------------------------------------------------------*/

#include "fixedFluxPressureFvPatchScalarField.H"
#include "fvPatchFieldMapper.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fixedFluxPressureFvPatchScalarField::fixedFluxPressureFvPatchScalarField
(
	const fvPatch& p,
	const DimensionedField<scalar, volMesh>& iF
)
:
	fixedGradientFvPatchScalarField(p, iF),
	UName_("U"),
	phiName_("phi"),
	rhoName_("rho"),
	adjoint_(false)
{}


Foam::fixedFluxPressureFvPatchScalarField::fixedFluxPressureFvPatchScalarField
(
	const fixedFluxPressureFvPatchScalarField& ptf,
	const fvPatch& p,
	const DimensionedField<scalar, volMesh>& iF,
	const fvPatchFieldMapper& mapper
)
:
	fixedGradientFvPatchScalarField(ptf, p, iF, mapper),
	UName_(ptf.UName_),
	phiName_(ptf.phiName_),
	rhoName_(ptf.rhoName_),
	adjoint_(ptf.adjoint_)
{}


Foam::fixedFluxPressureFvPatchScalarField::fixedFluxPressureFvPatchScalarField
(
	const fvPatch& p,
	const DimensionedField<scalar, volMesh>& iF,
	const dictionary& dict
)
:
	fixedGradientFvPatchScalarField(p, iF),
	UName_(dict.lookupOrDefault<word>("U", "U")),
	phiName_(dict.lookupOrDefault<word>("phi", "phi")),
	rhoName_(dict.lookupOrDefault<word>("rho", "rho")),
	adjoint_(dict.lookup("adjoint"))
{
	if (dict.found("gradient"))
	{
		gradient() = scalarField("gradient", dict, p.size());
		fixedGradientFvPatchScalarField::updateCoeffs();
		fixedGradientFvPatchScalarField::evaluate();
	}
	else
	{
		fvPatchField<scalar>::operator=(patchInternalField());
		gradient() = 0.0;
	}
}


Foam::fixedFluxPressureFvPatchScalarField::fixedFluxPressureFvPatchScalarField
(
	const fixedFluxPressureFvPatchScalarField& wbppsf
)
:
	fixedGradientFvPatchScalarField(wbppsf),
	UName_(wbppsf.UName_),
	phiName_(wbppsf.phiName_),
	rhoName_(wbppsf.rhoName_),
	adjoint_(wbppsf.adjoint_)
{}


Foam::fixedFluxPressureFvPatchScalarField::fixedFluxPressureFvPatchScalarField
(
	const fixedFluxPressureFvPatchScalarField& wbppsf,
	const DimensionedField<scalar, volMesh>& iF
)
:
	fixedGradientFvPatchScalarField(wbppsf, iF),
	UName_(wbppsf.UName_),
	phiName_(wbppsf.phiName_),
	rhoName_(wbppsf.rhoName_),
	adjoint_(wbppsf.adjoint_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::fixedFluxPressureFvPatchScalarField::updateCoeffs()
{
	if (updated())
	{
		return;
	}

	const fvPatchField<vector>& Up =
		lookupPatchField<volVectorField, vector>(UName_);

	const surfaceScalarField& phi =
		db().lookupObject<surfaceScalarField>(phiName_);

	fvsPatchField<scalar> phip =
		patch().patchField<surfaceScalarField, scalar>(phi);

	if (phi.dimensions() == dimDensity*dimVelocity*dimArea)
	{
		const fvPatchField<scalar>& rhop =
			lookupPatchField<volScalarField, scalar>(rhoName_);

		phip /= rhop;
	}

	const fvPatchField<scalar>& rAp =
		lookupPatchField<volScalarField, scalar>("(1|A("+UName_+"))");

	if (adjoint_)
	{
		gradient() = ((patch().Sf() & Up) - phip)/patch().magSf()/rAp;
	}
	else
	{
		gradient() = (phip - (patch().Sf() & Up))/patch().magSf()/rAp;
	}

	fixedGradientFvPatchScalarField::updateCoeffs();
}


void Foam::fixedFluxPressureFvPatchScalarField::write(Ostream& os) const
{
	fvPatchScalarField::write(os);
	if (UName_ != "U")
	{
		os.writeKeyword("U") << UName_ << token::END_STATEMENT << nl;
	}
	if (phiName_ != "phi")
	{
		os.writeKeyword("phi") << phiName_ << token::END_STATEMENT << nl;
	}
	if (rhoName_ != "rho")
	{
		os.writeKeyword("rho") << rhoName_ << token::END_STATEMENT << nl;
	}
	os.writeKeyword("adjoint") << adjoint_ << token::END_STATEMENT << nl;
	gradient().writeEntry("gradient", os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
	makePatchTypeField
	(
		fvPatchScalarField,
		fixedFluxPressureFvPatchScalarField
	);
}

// ************************************************************************* //
