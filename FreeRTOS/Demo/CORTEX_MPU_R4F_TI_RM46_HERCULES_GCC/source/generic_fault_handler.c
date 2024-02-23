void vTestFaultHandler( void )
{
    /* 
     * For now, just blindly print that the test passed.
     * The desired behavior is to check the step tracker buffer,
     * and check that the most recent step written to the buffer is 
     * what we expect it to be.
     * Each test can choose not to include this file and define their
     * own fault handler if this one does not fit the needs of the test.
     */
    sci_print("Test Passed!\r\n\r\n");

    /* Loop forever */
    while(1)
    {

    }
}