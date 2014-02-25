# Drush Typeinfo

Get information about entity bundles and their fields.

## Installation

    pushd ~/.drush
    git clone git@github.com:robballou/drush_typeinfo.git

## Usage

List all entity types and their bundles using `typeinfo-list` (`til`):

    drush typeinfo-list

List all fields for the node/article bundle with the `typeinfo` (`ti`):

    drush typeinfo article

List all the fields for the taxonomy_term/location bundle:

    drush typeinfo location taxonomy_term
