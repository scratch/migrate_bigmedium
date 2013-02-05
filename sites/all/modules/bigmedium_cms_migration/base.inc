<?php

/**
 * @file
 * Defines the base class for all the migrations.
 */

/**
 *
 * All migration settings are defined here.
 *
 */
abstract class BaseMigration extends Migration {
    public $queryParams = array();

    public function __construct() {
      // Always call the parent constructor first for basic setup
      parent::__construct();
      $this->team = array(
        new MigrateTeamMember('Test', 'test@srijan.in', t('Product Owner')),
      );

    $this->issuePattern = 'http://drupal.org/node/:id';

  }

}
