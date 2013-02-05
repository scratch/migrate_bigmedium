<?php
/**
 * @file
 * Migration script for users migration.
 */

/**
 *
 * Migrate Backend Users
 */
class UserMigration extends BaseMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Data migration from static shtml.');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'sourceid' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        )
      ),
      MigrateDestinationNode::getKeySchema()
    );

    $fields = array(
      'name' => t('Username'),
      'email' => t('Email'),
      'password' => t('Password'),
    );

    $data_folder = DRUPAL_ROOT . '/' . drupal_get_path('module', 'bigmedium_cms_migration') . '/data/users/';
    $list_url = $data_folder . 'index.cgi';
    // Each ID retrieved from the list URL will be plugged into :id in the
    // item URL to fetch the specific objects.
    $item_url = $data_folder . ':id.cgi';
    $this->source = new MigrateSourceList(new MigrateListCGI($list_url, NULL),
      new MigrateItemCGI($item_url), $fields);

    $this->destination = new MigrateDestinationUser(array('md5_passwords' => TRUE));

    $this->addFieldMapping('name', 'name')->dedupe('users', 'name');
    $this->addFieldMapping('mail', 'email');
    $this->addFieldMapping('pass', 'password');
    $this->addFieldMapping('is_new')
      ->defaultValue(TRUE);
    $this->addFieldMapping('roles', 'role')
      ->defaultValue(2); // authenticated user
    $this->addFieldMapping('theme')
      ->defaultValue('');
    $this->addFieldMapping('signature')
      ->defaultValue('');
    $this->addFieldMapping('signature_format')
      ->defaultValue('filtered_html');
    $this->addFieldMapping('created')
      ->defaultValue(time());
    $this->addFieldMapping('access')
      ->defaultValue(0);
    $this->addFieldMapping('login')
      ->defaultValue(0);
    $this->addFieldMapping('status')
      ->defaultValue(1);
    $this->addFieldMapping('timezone')
      ->defaultValue(NULL);
    $this->addFieldMapping('language')
      ->defaultValue(t('en')); // e.g.: 'en', 'fr', 'de', ...
    $this->addFieldMapping('picture')
      ->defaultValue(0);
    $this->addFieldMapping('init', 'mail');
  }

  public function prepareRow($row) {
   // print '<pre>';print_r($row);die();
    $row->name = $row->filedata['name'];
    $row->email = $row->filedata['email'];
    //$row->password = '$P$'.$row->filedata['password'];
	require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');
	$old_hash = ('1234');
	$hash_count_log2 = 0;

	$new_hash = user_hash_password($old_hash, $hash_count_log2);

	if ($new_hash) {
	  // Indicate an updated password.
	  $row->password  =  $new_hash;
	}
    //print '<pre>';print_r($row);die();
  }
}


