FROM postgres:16-alpine

# Set default environment variables
ENV POSTGRES_DB=plantnanny
ENV POSTGRES_USER=plantnanny
ENV POSTGRES_PASSWORD=plantnanny_secret

# Expose PostgreSQL port
EXPOSE 5432

# Health check
HEALTHCHECK --interval=10s --timeout=5s --start-period=30s --retries=3 \
    CMD pg_isready -U $POSTGRES_USER -d $POSTGRES_DB || exit 1
